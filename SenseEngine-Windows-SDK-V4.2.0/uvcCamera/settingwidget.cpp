#include <QDebug>
#include "settingwidget.h"
#include "ui_settingwidget.h"
#include "ai_camera.h"

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle("Setting");
    //connect(ui->cameraListBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&SettingWidget::selectPairPort);
    this->initCameraInfo();
    connect(ui->applyButton,&QPushButton::clicked,this,&SettingWidget::applySlots);
    connect(&m_timer,&QTimer::timeout,this,&SettingWidget::initCameraInfo);
    //connect(&m_timer,&QTimer::timeout,this,&SettingWidget::initPortsInfo);
    this->m_timer.start(500);
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::applySlots()
{
    this->hide();
    QString deviceName = ui->cameraListBox->currentText();
    QString serialName = ui->serialPortListBox->currentText();
    emit this->applyCamera(deviceName,serialName);
    this->close();
}

void SettingWidget::initCameraInfo()
{    
    QByteArray camArray    = QByteArray(1024,0x00);
    QByteArray serialArray = QByteArray(255,0x00);
    EnumDevice(camArray.data(),1024,serialArray.data(),255);

    //qDebug() << "CameraName" << QString(camArray);
    //qDebug() << "SerialName" << QString(serialArray);

    QStringList cameraNameList = QString(camArray).split('|',QString::SkipEmptyParts);
    QStringList serialNameList = QString(serialArray).split('|',QString::SkipEmptyParts);

    if(cameraNameList.size() == ui->cameraListBox->count() &&
       serialNameList.size() == ui->serialPortListBox->count())
    {
        return;
    }

    ui->cameraListBox->clear();
    ui->serialPortListBox->clear();
    ui->cameraListBox->addItems(cameraNameList);
    ui->serialPortListBox->addItems(serialNameList);
}
