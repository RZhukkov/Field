

#include "glwidget.h"
#include "window.h"
#include "settingswindow.h"
#include "field.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFile>
#include <QString>
#include <QCoreApplication>
#include <QDir>

Window::Window(FieldSetting stng, SettingsWindow *mw)
{
    glWidget = new GLWidget(stng);

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    connect(xSlider, &QSlider::valueChanged, glWidget, &GLWidget::setXRotation);
    connect(glWidget, &GLWidget::xRotationChanged, xSlider, &QSlider::setValue);
    connect(ySlider, &QSlider::valueChanged, glWidget, &GLWidget::setYRotation);
    connect(glWidget, &GLWidget::yRotationChanged, ySlider, &QSlider::setValue);
    connect(zSlider, &QSlider::valueChanged, glWidget, &GLWidget::setZRotation);
    connect(glWidget, &GLWidget::zRotationChanged, zSlider, &QSlider::setValue);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(glWidget);
    container->addWidget(xSlider);
    container->addWidget(ySlider);
    container->addWidget(zSlider);

    QWidget *w = new QWidget;
    w->setLayout(container);
    mainLayout->addWidget(w);

    setLayout(mainLayout);

    xSlider->setValue(15 * 16);
    ySlider->setValue(345 * 16);
    zSlider->setValue(0 * 16);

    setWindowTitle("Visualization");

    m_pmnu = new QMenu(this);
    m_pmnu->addAction("Сохранить данные расчетов");
    m_pmnu->addSeparator();
    m_pmnu->addAction("Exit");
    connect(m_pmnu, SIGNAL(triggered(QAction*)), this, SLOT(SaveInformation(QAction *)));
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

Window::~Window()
{
    delete createSlider();
    delete glWidget;
    delete xSlider;
    delete ySlider;
    delete zSlider;
}

void  Window::SaveInformation(QAction* action)
{
    if (action->text()=="Exit") this->close();
    else
    {
        QString pathApp = QCoreApplication::applicationDirPath()+ QDir::separator();
        QFile file(pathApp+"VisualisationData.txt");
        if (file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QTextStream outStream (&file);  //Создание объекта текстового потока
            outStream.setCodec("UTF-8");
            outStream<<"Ishodnie dannie:\n";
            outStream<<"f="<<glWidget->get_logo().getField().get_freq()<<"\teps="<<glWidget->get_logo().getField().get_eps();
            outStream<<"\tmu="<<glWidget->get_logo().getField().get_mu()<<"\tA="<<glWidget->get_logo().getField().get_A();
            outStream<<" B="<<glWidget->get_logo().getField().get_B()<<" Z="<<glWidget->get_logo().getField().get_Z();
            outStream<<" E0="<<glWidget->get_logo().getField().get_E0()<<" H0="<<glWidget->get_logo().getField().get_H0();
            outStream<<"\nRaschetnie dannie:\n";

            list <mody> ::iterator IT;

            for (IT=glWidget->get_logo().getField().get_omega_krit().begin(); IT!=glWidget->get_logo().getField().get_omega_krit().end(); IT++)
            {
                outStream<<"m="<<IT->m<<"\tn="<<IT->n<<"\tomega krit="<<IT->omega_krit<<"\n";
            }
            QMessageBox::information(0,"Успешно", "Данные сохранены в файле "+pathApp+"VisualisationData.txt");
        }
        else {QMessageBox::warning(0, "Внимание!!!", "Ошибка при открытии файла!");}
        file.close();
    }
}
