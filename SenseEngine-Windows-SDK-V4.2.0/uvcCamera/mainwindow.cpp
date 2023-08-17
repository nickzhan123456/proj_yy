/************************************************************************
Copyright  : 2018-2028,Shenzhen SenseTime Co.,Ltd
File name  : mainwindow.cpp
Description: .
Author     : longzeyuan
Date       : 2019-08-01
Version    : V1.0.5
History    : 2019-07-22, Create it by longzeyuan
************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
//#include <QSerialPortInfo>
#include <QFileDialog>
#include <QFileInfo>
#include<QInputDialog>
#include <QThread>
#include <QDir>
#include "XFFmpeg.h"
#include "XVideoThread.h"
#include "addbatchwidget.h"
#include "upgradewidget.h"
#include "settingwidget.h"
#include "qmtc/mtcmanager.h"
#include "inputdialog.h"
#include "setrecognizecount.h"
#include "irlightconfigdialog.h"
#include "qrcodeswitchdialog.h"
#include "logsettingdialog.h"
#include "antiflickerdialog.h"
#include <QDebug>
#include <QJsonDocument>

//Q_DECLARE_METATYPE(QCameraInfo)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    ,m_serailStatus(new QLabel)
    ,m_cameraStatus(new QLabel)
    ,m_devReopenTip(new QLabel)
{    
    ui->setupUi(this);
    //qDebug() << "MainWindow::MainWindow in ";
    this->setWindowTitle("SDK SenseEngine Camera MtcLib Edition");
    QIcon icon(":/resource/sensetime.png");
    QPixmap pixmap = icon.pixmap(QSize(56, 56));
    this->setWindowIcon(pixmap);//QIcon(":/resource/sensetime.png"));
    ui->statusBar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
    ui->statusBar->addWidget(m_cameraStatus);
    ui->statusBar->addWidget(new QLabel(" "));
    ui->statusBar->addWidget(m_serailStatus);
    ui->statusBar->addPermanentWidget(m_devReopenTip);
    ui->menuBar->setVisible(true);
    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    qRegisterMetaType<DeviceType>("DeviceType");
    connect(&this->m_msgTimer,&QTimer::timeout,this,&MainWindow::msgLabelTimerOut);
    connect(ui->actionSetting,&QAction::triggered,this,&MainWindow::settingWidget);
    connect(ui->actionDisConnection,&QAction::triggered,this,&MainWindow::disConnectionDev);
    connect(ui->actionAbout,&QAction::triggered,this, &MainWindow::appAboutSlot);
    connect(ui->actionAdd_base_library_by_Camera,&QAction::triggered,this,&MainWindow::addBaseLibraryByCamera);
    connect(ui->actionAdd_base_library_by_Camera_return,&QAction::triggered,this,&MainWindow::addBaseLibraryByCamera_return);
    connect(ui->actionAdd_base_library,&QAction::triggered,this,&MainWindow::addBaseLibrary);
    connect(ui->actionBatch_addition,&QAction::triggered,this,&MainWindow::addBatch);
    connect(ui->actionQuery_Single,&QAction::triggered,this,&MainWindow::querySingle);
    connect(ui->actionDelete,&QAction::triggered,this,&MainWindow::deleteOne);
    connect(ui->actionDelete_All,&QAction::triggered,this,&MainWindow::deleteAll);
    connect(ui->actionupgrade,&QAction::triggered,this,&MainWindow::upgrade);
    connect(ui->actionStart_Recognize,&QAction::triggered,this,&MainWindow::StartRecognize);
    connect(ui->actionStop_Recognize,&QAction::triggered,this,&MainWindow::StopRecognize);
    connect(ui->actionQuery_Recognize_Status,&QAction::triggered,this,&MainWindow::QueryRecognizeStatus);

    connect(ui->actionTrue_Mirrored,&QAction::triggered,this,&MainWindow::setIsMirroredTrue);
    connect(ui->actionFalse_Mirrored,&QAction::triggered,this,&MainWindow::setIsMirroredFalse);

    //1:N
    connect(ui->actionMulti_Living,&QAction::triggered,this,&MainWindow::MultiLivingMode);
    connect(ui->actionMulti_Noliving,&QAction::triggered,this,&MainWindow::MultiNolivingMode);
    connect(ui->actionSingle_Living,&QAction::triggered,this,&MainWindow::SingleLivingMode);
    connect(ui->actionSingle_Noliving,&QAction::triggered,this,&MainWindow::SingleNolivingMode);
    //1:1
    connect(ui->actionOne_Multi_Living,&QAction::triggered,this,&MainWindow::OtoMultiLivingMode);
    connect(ui->actionOne_Multi_Noliving,&QAction::triggered,this,&MainWindow::OtoMultiNolivingMode);
    connect(ui->actionOto_Single_Living,&QAction::triggered,this,&MainWindow::OtoSingleLivingMode);
    connect(ui->actionOto_Single_Noliving,&QAction::triggered,this,&MainWindow::OtoSingleNolivingMode);

    connect(ui->actionGet_Version,&QAction::triggered,this,&MainWindow::GetVersion);
    connect(ui->actionGet_Library_2,&QAction::triggered,this,&MainWindow::GetLibrary);
    connect(ui->actionSwitch_Camera_RGB,&QAction::triggered,this,&MainWindow::SwitchCameraRGB);
    connect(ui->actionSwitch_Camera_IR,&QAction::triggered,this,&MainWindow::SwitchCameraIR);    
    connect(ui->actionLocal_Capture,&QAction::triggered,this,&MainWindow::StaticCompareLocalCapture);
    connect(ui->actionRemote_Input,&QAction::triggered,this,&MainWindow::StaticCompareRemoteInput);
    connect(ui->actionAdd_feature,&QAction::triggered,this,&MainWindow::addFeature);
    connect(ui->actionGet_feature,&QAction::triggered,this,&MainWindow::queryFeature);
    connect(ui->actionSet_Frame,&QAction::triggered,this,&MainWindow::setFrame);
    connect(ui->action_IR_Light_Config,&QAction::triggered,this,&MainWindow::getIrLight);
    connect(ui->actionLog_Config,&QAction::triggered,this,&MainWindow::getLogConfig);
    connect(ui->actionSwitch_QRCode,&QAction::triggered,this,&MainWindow::setSwitchQRCode);
    connect(ui->actionGet_Cpu_Temp,&QAction::triggered,this,&MainWindow::getCpuTemperature);
    connect(ui->actionGet_AE_Sensor, &QAction::triggered, this, &MainWindow::getAESensorModel);

    connect(ui->actionSet_Recognize_Count,&QAction::triggered,this,&MainWindow::setRecognizeCount);
    connect(ui->actionGet_Recognize_Count,&QAction::triggered,this,&MainWindow::getRecognizeCount);

    connect(ui->actionOpen_TemplateUpdate,&QAction::triggered,this,&MainWindow::openTemlateUpdate);
    connect(ui->actionClose_TemplateUpdate,&QAction::triggered,this,&MainWindow::closeTemlateUpdate);
    connect(ui->actionGet_Template_Update,&QAction::triggered,this,&MainWindow::getTemlateUpdate);

    connect(ui->action_AI_Config,&QAction::triggered,this,&MainWindow::getAiConfig);
    connect(ui->actionSet_Video_Size,&QAction::triggered,this,&MainWindow::setVideoSize);
    connect(ui->actionSet_Screen_Mode,&QAction::triggered,this,&MainWindow::setScreenMode);
    connect(ui->actionSet_Rotation,&QAction::triggered,this,&MainWindow::setRotation);
    connect(ui->actionGet_Sense_Value,&QAction::triggered,this,&MainWindow::getLightSensor);
    connect(ui->actionRecovery,&QAction::triggered,this,&MainWindow::recovery);
    connect(ui->actionReboot_2,&QAction::triggered,this,&MainWindow::reboot);
    connect(ui->actionUser_Reboot_with_Udisk,&QAction::triggered,this,&MainWindow::userRebootWithDisk);
    connect(ui->actionUser_Reboot_without_Udisk,&QAction::triggered,this,&MainWindow::userRebootWithoutDisk);
    connect(ui->actionFactory_Reboot_with_Udisk,&QAction::triggered,this,&MainWindow::factoryRebootWithDisk);
    connect(ui->actionFactory_Reboot_without_Udisk,&QAction::triggered,this,&MainWindow::factoryRebootWithoutDisk);
    connect(ui->actionGet_SN,&QAction::triggered,this,&MainWindow::getSN);
    connect(ui->actionGet_Model,&QAction::triggered,this,&MainWindow::getModel);
    connect(ui->actionOpen_RGB,&QAction::triggered,this,&MainWindow::openRGBCamera);
    connect(ui->actionOpen_IR,&QAction::triggered,this,&MainWindow::openIRCamera);
    connect(ui->actionClose_RGB,&QAction::triggered,this,&MainWindow::closeRGBCamera);
    connect(ui->actionClose_IR,&QAction::triggered,this,&MainWindow::closeIRCamera);
    connect(ui->actionGet_ID_List,&QAction::triggered,this,&MainWindow::getIDList);
    connect(ui->actionGet_Single_Recognize,&QAction::triggered,this,&MainWindow::getSingleRecognize);

    //connect(ui->actionOpen_Auto_Report,&QAction::triggered,this,&MainWindow::openAutoReport);
    connect(ui->actionfaceRGB_backRGB_IR,&QAction::triggered,this,&MainWindow::openAutoReport_faceRGB_backRGB_IR);
    connect(ui->actionfaceRGB_backRGB,&QAction::triggered,this,&MainWindow::openAutoReport_faceRGB_backRGB);
    connect(ui->actionfaceRGB_IR,&QAction::triggered,this,&MainWindow::openAutoReport_faceRGB_IR);
    connect(ui->actionbackRGB_IR,&QAction::triggered,this,&MainWindow::openAutoReport_backRGB_IR);
    connect(ui->actionfaceRGB,&QAction::triggered,this,&MainWindow::openAutoReport_faceRGB);
    connect(ui->actionbackRGB,&QAction::triggered,this,&MainWindow::openAutoReport_backRGB);
    connect(ui->actionIR,&QAction::triggered,this,&MainWindow::openAutoReport_IR);
    connect(ui->actionNoImage,&QAction::triggered,this,&MainWindow::openAutoReport_NoImage);

    connect(ui->actionClose_Auto_Report,&QAction::triggered,this,&MainWindow::closeAutoReport);



    connect(ui->actionOpen_Frame_Report,&QAction::triggered,this,&MainWindow::openFrameReport);
    connect(ui->actionClose_Frame_Report,&QAction::triggered,this,&MainWindow::closeFrameReport);

    connect(ui->actionRGB_50HZ,&QAction::triggered,this,&MainWindow::antiFlickerEnableRGB50);
    connect(ui->actionRGB_60HZ,&QAction::triggered,this,&MainWindow::antiFlickerEnableRGB60);
    connect(ui->actionIR_50HZ,&QAction::triggered,this,&MainWindow::antiFlickerEnableIR50);
    connect(ui->actionIR_60HZ,&QAction::triggered,this,&MainWindow::antiFlickerEnableIR60);
    connect(ui->actionRGB_50HZ_2,&QAction::triggered,this,&MainWindow::antiFlickerDisenableRGB50);
    connect(ui->actionRGB_60HZ_2,&QAction::triggered,this,&MainWindow::antiFlickerDisenableRGB60);
    connect(ui->actionIR_50HZ_2,&QAction::triggered,this,&MainWindow::antiFlickerDisenableIR50);
    connect(ui->actionIR_60HZ_2,&QAction::triggered,this,&MainWindow::antiFlickerDisenableIR60);

    connect(ui->actionRGB_Camera,&QAction::triggered,this,&MainWindow::antiFlickerRGBCamera);
    connect(ui->actionIR_Camera,&QAction::triggered,this,&MainWindow::antiFlickerIRCamera);

    connect(ui->actionSetUmodeToEngin,&QAction::triggered,this,&MainWindow::setUmodeToEngin);
    connect(ui->actionReset,&QAction::triggered,this,&MainWindow::reset);
    connect(ui->actionSet_Stream_Format,&QAction::triggered,this,&MainWindow::setStreamFormat);
    connect(ui->actionGet_Model_App_Version,&QAction::triggered,this,&MainWindow::getModelAppVersion);

    connect(MtcManager::instance(),&MtcManager::portStatusMsgSignal,this,&MainWindow::showStatusMessage);
    connect(MtcManager::instance(),&MtcManager::errorSignal,this,&MainWindow::errorSlot);

    connect(MtcManager::instance(),&MtcManager::openFrameAutoResult,this,&MainWindow::openFrameAutoResult);
    connect(MtcManager::instance(),&MtcManager::closeFrameAutoResult,this,&MainWindow::closeFrameAutoResult);
    connect(MtcManager::instance(),&MtcManager::setDeviceNoFlickerHzSignal,this,&MainWindow::setDeviceNoFlickerHzSignalResult);
    connect(MtcManager::instance(),&MtcManager::getDeviceNoFlickerHzSignal,this,&MainWindow::getDeviceNoFlickerHzSignalResult);

    connect(MtcManager::instance(),&MtcManager::pingResult,this,&MainWindow::pingResult);
    connect(MtcManager::instance(),&MtcManager::checkModelM20,this,&MainWindow::checkModelM20);
    connect(MtcManager::instance(),&MtcManager::queryFaceResult,this,&MainWindow::queryFaceResultSlot);
    connect(MtcManager::instance(),&MtcManager::deleteImageResult,this,&MainWindow::deleteImageResultSlot);
    connect(MtcManager::instance(),&MtcManager::addImageResult,this,&MainWindow::addImageResultSlot);
    connect(MtcManager::instance(),&MtcManager::inputLibraryResult,this,&MainWindow::inputLibraryResultSlot);
    connect(MtcManager::instance(),&MtcManager::startRecognizeResult,this,&MainWindow::startRecognizeResultSlot);
    connect(MtcManager::instance(),&MtcManager::stopRecognizeResult,this,&MainWindow::stopRecognizeResultSlot);
    connect(MtcManager::instance(),&MtcManager::queryRecognizeStatusResult,this,&MainWindow::queryRecognizeStatusResultSlot);
    connect(MtcManager::instance(),&MtcManager::onetooneRecognizeResult,this,&MainWindow::onetooneRecognizeResultSlot);
    connect(MtcManager::instance(),&MtcManager::queryFeatureResult,this,&MainWindow::queryFeatureResultSlot);
    connect(MtcManager::instance(),&MtcManager::getIDListResult,this,&MainWindow::getIDListResultSlot);
    connect(MtcManager::instance(),&MtcManager::openAutoReportResult,this,&MainWindow::openAutoReportResultSlot);
    connect(MtcManager::instance(),&MtcManager::closeAutoReportResult,this,&MainWindow::closeAutoReportResultSlot);
    connect(MtcManager::instance(),&MtcManager::setFrameResult,this,&MainWindow::setFrameResultSlot);
    connect(MtcManager::instance(),&MtcManager::getFrameResult,this,&MainWindow::getFrameResultSlot);
    connect(MtcManager::instance(),&MtcManager::setIrLightResult,this,&MainWindow::setIrLightResultSlot);
    connect(MtcManager::instance(),&MtcManager::getIrLightResult,this,&MainWindow::getIrLightResultSlot);
    connect(MtcManager::instance(),&MtcManager::setLogConfigResult,this,&MainWindow::setLogConfigResultSlot);
    connect(MtcManager::instance(),&MtcManager::getLogConfigResult,this,&MainWindow::getLogConfigResultSlot);
    connect(MtcManager::instance(),&MtcManager::setRecognizeCountResult,this,&MainWindow::setRecognizeCountResultSlot);
    connect(MtcManager::instance(),&MtcManager::getRecognizeCountResult,this,&MainWindow::getRecognizeCountResultSlot);
    connect(MtcManager::instance(),&MtcManager::setTemlateUpdateResult,this,&MainWindow::setTemlateUpdateResultSlot);
    connect(MtcManager::instance(),&MtcManager::getTemlateUpdateResult,this,&MainWindow::getTemlateUpdateResultSlot);
    connect(MtcManager::instance(),&MtcManager::setQRCodeSwitchResult,this,&MainWindow::setQRCodeSwitchResultSlot);
    connect(MtcManager::instance(),&MtcManager::qrCodeReportSignal,this,&MainWindow::qrCodeReportSlot);
    connect(MtcManager::instance(),&MtcManager::getCpuTemperatureResult,this,&MainWindow::getCpuTemperatureSlot);
    connect(MtcManager::instance(),&MtcManager::getAESensorModelResult, this, &MainWindow::getAESensorModelSlot);

    connect(MtcManager::instance(),&MtcManager::getLightSensorResult,this,&MainWindow::getLightSensorResultSlot);
    connect(MtcManager::instance(),&MtcManager::setVideoSizeResult,this,&MainWindow::setVideoSizeResultSlot);
    connect(MtcManager::instance(),&MtcManager::getVideoSizeResult,this,&MainWindow::getVideoSizeResultSlot);
    connect(MtcManager::instance(),&MtcManager::setScreenModeResult,this,&MainWindow::setScreenModeResultSlot);
    connect(MtcManager::instance(),&MtcManager::setRotateAngleResult,this,&MainWindow::setRotateAngleResultSlot);
    connect(MtcManager::instance(),&MtcManager::setAiConfigResult,this,&MainWindow::setAiConfigResultSlot);
    connect(MtcManager::instance(),&MtcManager::getAiConfigResult,this,&MainWindow::getAiConfigResultSlot);
    connect(MtcManager::instance(),&MtcManager::getVersionResult,this,&MainWindow::getVersionResultSlot);
    connect(MtcManager::instance(),&MtcManager::getSingleRecognizeResult,this,&MainWindow::getSingleRecognizeResultSlot);
    connect(MtcManager::instance(),&MtcManager::getSnResult,this,&MainWindow::getSnResultSlot);
    connect(MtcManager::instance(),&MtcManager::getModelResult,this,&MainWindow::getModelResultSlot);
    connect(MtcManager::instance(),&MtcManager::getLibraryResult,this,&MainWindow::getLibraryResult);
    connect(MtcManager::instance(),&MtcManager::switchCameraRgbResult,this,&MainWindow::switchCameraRgbResultSlot);
    connect(MtcManager::instance(),&MtcManager::switchCameraIrResult,this,&MainWindow::switchCameraIrResultSlot);
    connect(MtcManager::instance(),&MtcManager::rebootResult,this,&MainWindow::rebootResultSlot);
    connect(MtcManager::instance(),&MtcManager::recoveryResult,this,&MainWindow::recoveryResultSlot);
    connect(MtcManager::instance(),&MtcManager::opencameraResult,this,&MainWindow::opencameraResultSlot);
    connect(MtcManager::instance(),&MtcManager::closecameraResult,this,&MainWindow::closecameraResultSlot);

    connect(MtcManager::instance(),&MtcManager::setUmodeToEnginResult,this,&MainWindow::setUmodeToEnginResultSlot);
    connect(MtcManager::instance(),&MtcManager::resetResult,this,&MainWindow::resetResultSlot);
    connect(MtcManager::instance(),&MtcManager::setStreamFormatResult,this,&MainWindow::setStreamFormatResultSlot);
    connect(MtcManager::instance(),&MtcManager::getModelAppVersionResult,this,&MainWindow::getModelAppVersionResultSlot);

    this->setActionEnable(false);    
    MtcManager::instance()->open();
}

void MainWindow::showStatusMessage(const bool &status, const QString &message)
{
    m_serailStatus->setText(message);    
    setActionEnable(status);
    if(status)  m_devReopenTip->clear();
}

void MainWindow::settingWidget()
{
    SettingWidget *settingWidget = new SettingWidget(this);
    connect(settingWidget,&SettingWidget::applyCamera,this,&MainWindow::setCamera);
    settingWidget->show();
}

void MainWindow::disConnectionDev()
{
    XVideoThread::Get()->Close();
    XFFmpeg::Get()->Close();
    MtcManager::instance()->closePort();
    setActionEnable(false);
    m_showMsgDialog = false;
    m_isInitCamera = false;
    m_isGetVersion = false;
    m_isOpenRGB    = false;
    m_serailStatus->clear();
    m_cameraStatus->clear();
    m_devReopenTip->setText("Please connect camera and serial again");

    m_qrcodeStatus   = 0;//0：OFF，1：ON
    m_qrcodeInterval = 2;
    if(m_qrcode_file != nullptr)
    {
        m_qrcode_file->close();
        m_qrcode_file->deleteLater();
        m_qrcode_file = nullptr;
    }
}

void MainWindow::appAboutSlot()
{
    QMessageBox::about(this,"SenseEnginie Camera MtcLib Edition","Version: V4.2.0.2");
}
void MainWindow::StartRecognize()
{
    MtcManager::instance()->MtcManager::StartRecognize(MTC_TAG_RESUME_RECOGNIZE);    
    XFFmpeg::Get()->setFaceRectEnable(m_isOpenRGB);
}
void MainWindow::StopRecognize()
{
    MtcManager::instance()->MtcManager::StopRecognize(MTC_TAG_PAUSE_RECOGNIZE);
    XFFmpeg::Get()->setFaceRectEnable(false);
}

void MainWindow::QueryRecognizeStatus()
{
    MtcManager::instance()->MtcManager::QueryRecognizeStatus();
}

void MainWindow::setIsMirroredTrue()
{
    XFFmpeg::Get()->setIsMirrored(true);
}

void MainWindow::setIsMirroredFalse()
{
    XFFmpeg::Get()->setIsMirrored(false);
}

void MainWindow::msgLabelTimerOut()
{
    ui->msgLabel->setText("");
    m_msgTimer.stop();
}

void MainWindow::MultiLivingMode()
{
    MtcManager::instance()->MtcManager::onetonumReomode(MTC_LIVING_MODE, MTC_MULTI_MODE);
}

void MainWindow::MultiNolivingMode()
{
    MtcManager::instance()->MtcManager::onetonumReomode(MTC_NOLIVING_MODE, MTC_MULTI_MODE);
}

void MainWindow::SingleLivingMode()
{
    MtcManager::instance()->MtcManager::onetonumReomode(MTC_LIVING_MODE, MTC_SINGLE_MODE);
}

void MainWindow::SingleNolivingMode()
{
    MtcManager::instance()->MtcManager::onetonumReomode(MTC_NOLIVING_MODE, MTC_SINGLE_MODE);
}

void MainWindow::OtoMultiLivingMode()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Base Library"),
                                "/home",
                                tr("JPEG Files (*.jpg *.jpeg)"),
                                &selectedFilter);
    if (!fileName.isEmpty())
    {
            MtcManager::instance()->onetooneFile(MTC_LIVING_MODE, MTC_MULTI_MODE, fileName);
    }
}
void MainWindow::OtoMultiNolivingMode()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Base Library"),
                                "/home",
                                tr("JPEG Files (*.jpg *.jpeg)"),
                                &selectedFilter);
    if (!fileName.isEmpty())
    {
            MtcManager::instance()->onetooneFile(MTC_NOLIVING_MODE, MTC_MULTI_MODE, fileName);
    }
}
void MainWindow::OtoSingleLivingMode()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Base Library"),
                                "/home",
                                tr("JPEG Files (*.jpg *.jpeg)"),
                                &selectedFilter);
    if (!fileName.isEmpty())
    {
            MtcManager::instance()->onetooneFile(MTC_LIVING_MODE, MTC_SINGLE_MODE, fileName);
    }
}
void MainWindow::OtoSingleNolivingMode()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Base Library"),
                                "/home",
                                tr("JPEG Files (*.jpg *.jpeg)"),
                                &selectedFilter);
    if (!fileName.isEmpty())
    {
        MtcManager::instance()->onetooneFile(MTC_NOLIVING_MODE, MTC_SINGLE_MODE, fileName);
    }
}
void MainWindow::GetVersion()
{
    MtcManager::instance()->MtcManager::GetVersion();
}

void MainWindow::getSingleRecognize()
{
    MtcManager::instance()->MtcManager::getSingleRecognize();
}

void MainWindow::getSN()
{
    MtcManager::instance()->MtcManager::getSN();
}

void MainWindow::getModel()
{
    MtcManager::instance()->MtcManager::getModel();
}

void MainWindow::GetLibrary()
{
    MtcManager::instance()->MtcManager::GetLibrary();
}
void MainWindow::SwitchCameraRGB()
{
    MtcManager::instance()->MtcManager::SwitchCameraRGB();
}
void MainWindow::SwitchCameraIR()
{
    MtcManager::instance()->MtcManager::SwitchCameraIR();
}

void MainWindow::openRGBCamera(int mode)
{
    MtcManager::instance()->MtcManager::OpenCamera(RGB_CAMERA);
}

void MainWindow::closeRGBCamera(int mode)
{
    MtcManager::instance()->MtcManager::CloseCamera(RGB_CAMERA);
}
void MainWindow::openIRCamera(int mode)
{
    MtcManager::instance()->MtcManager::OpenCamera(IR_CAMERA);
}

void MainWindow::closeIRCamera(int mode)
{
    MtcManager::instance()->MtcManager::CloseCamera(IR_CAMERA);
}

void MainWindow::getIDList()
{
    MtcManager::instance()->MtcManager::GetIDList();
}

void MainWindow::openAutoReport_faceRGB_backRGB_IR()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(7);
}

void MainWindow::openAutoReport_faceRGB_backRGB()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(3);
}

void MainWindow::openAutoReport_faceRGB_IR()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(5);
}

void MainWindow::openAutoReport_backRGB_IR()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(6);
}

void MainWindow::openAutoReport_faceRGB()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(1);
}

void MainWindow::openAutoReport_backRGB()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(2);
}

void MainWindow::openAutoReport_IR()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(4);
}

void MainWindow::openAutoReport_NoImage()
{
    MtcManager::instance()->MtcManager::OpenAutoReport(0);
}

void MainWindow::closeAutoReport()
{
    MtcManager::instance()->MtcManager::CloseAutoReport();
}

void MainWindow::openFrameReport()
{
    MtcManager::instance()->MtcManager::OpenFrameReport();
}
void MainWindow::closeFrameReport()
{
    MtcManager::instance()->MtcManager::CloseFrameReport();
}

void MainWindow::antiFlickerEnableRGB50()
{
    //0x00：Disable
    //0x01：Enable
    //0x00: Camera with ID 0 = RGB
    //0x01: Camera with ID 1 = IR
    //0x00: 50HZ
    //0x01: 60HZ
    MtcManager::instance()->MtcManager::antiFlicker(1,0,0);
}
void MainWindow::antiFlickerEnableRGB60()
{
    MtcManager::instance()->MtcManager::antiFlicker(1,0,1);
}
void MainWindow::antiFlickerEnableIR50()
{
    MtcManager::instance()->MtcManager::antiFlicker(1,1,0);
}
void MainWindow::antiFlickerEnableIR60()
{
    MtcManager::instance()->MtcManager::antiFlicker(1,1,1);
}

void MainWindow::antiFlickerDisenableRGB50()
{
    MtcManager::instance()->MtcManager::antiFlicker(0,0,0);
}
void MainWindow::antiFlickerDisenableRGB60()
{
    MtcManager::instance()->MtcManager::antiFlicker(0,0,1);
}
void MainWindow::antiFlickerDisenableIR50()
{
    MtcManager::instance()->MtcManager::antiFlicker(0,1,0);
}
void MainWindow::antiFlickerDisenableIR60()
{
    MtcManager::instance()->MtcManager::antiFlicker(0,1,1);
}

void MainWindow::antiFlickerRGBCamera()
{
    MtcManager::instance()->MtcManager::GetAntiFlicker(0);
}

void MainWindow::antiFlickerIRCamera()
{
    MtcManager::instance()->MtcManager::GetAntiFlicker(1);
}

void MainWindow::addBaseLibraryByCamera()
{
    bool ok = false;
    QString imageId = InputDialog::getText(this, tr("Get Image ID"),
                                         tr("Image ID:"),&ok);

    if (ok && !imageId.isEmpty())
    {        
        if(imageId.toUtf8().length() >32)
        {
            QMessageBox::warning(this,this->windowTitle(),"Beyond the input limit, please re-enter!");
            return;
        }        
        if(imageId.contains('\\') || imageId.contains('/') || imageId.contains(':') ||
           imageId.contains('*')  || imageId.contains('?') || imageId.contains('<') ||
           imageId.contains('>')  || imageId.contains('|') )
        {
            QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
            return;
        }

        //qDebug() << "add image by camera ";
        MtcManager::instance()->addBaseLibraryByCamera(imageId);
    }
    else
    {
        if(ok)
        {
            QMessageBox::warning(this,"SenseEnginie Camera","Input image id failed");
        }
    }
}

void MainWindow::addBaseLibraryByCamera_return()
{
    bool ok = false;
    QString imageId = InputDialog::getText(this, tr("Get Image ID"),
                                         tr("Image ID:"),&ok);

    if (ok && !imageId.isEmpty())
    {
        if(imageId.toUtf8().length() >32)
        {
            QMessageBox::warning(this,this->windowTitle(),"Beyond the input limit, please re-enter!");
            return;
        }
        int index = imageId.indexOf("\\");
        if(index != -1)
        {
            QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
            return;
        }
        if(imageId.contains("\"") || imageId.contains('/') || imageId.contains(':') ||
           imageId.contains('*')  || imageId.contains('?') || imageId.contains('<') ||
           imageId.contains('>')  || imageId.contains('|') )
        {
            QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
            return;
        }

        //qDebug() << "add image by camera ";
        MtcManager::instance()->addBaseLibraryByCameraReturn(imageId);
    }else
    {
        if(ok){

            QMessageBox::warning(this,"SenseEnginie Camera","Input image id failed");
        }
    }
}

void MainWindow::addBaseLibrary()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Base Library"),
                                "/home",
                                tr("JPEG Files (*.jpg *.jpeg)"),
                                &selectedFilter);
    QFileInfo Imageinfo(fileName);
    //qDebug() << "Image size is " << Imageinfo.size();
    if(Imageinfo.size()> (4*1024*1024))
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Beyond the Picture size limit,please input the picture below 4M!");
        return;
    }
    if (!fileName.isEmpty())
    {
        bool ok = false;
        QString imageId = InputDialog::getText(this, tr("Get Image ID"),
                                             tr("Image ID:"),&ok);
        if (ok && !imageId.isEmpty())
        {            
            if(imageId.contains('\\') || imageId.contains('/') || imageId.contains(':') ||
               imageId.contains('*')  || imageId.contains('?') || imageId.contains('<') ||
               imageId.contains('>')  || imageId.contains('|') )
            {
                QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
                return;
            }
            MtcManager::instance()->addImageFile(imageId,fileName);
        }
        else
        {
            if(ok){
                QMessageBox::warning(this,"SenseEnginie Camera","Input image id failed");
            }
        }
    }
}

void MainWindow::StaticCompareLocalCapture()
{
    MtcManager::instance()->inputLibrary();
}

void MainWindow::StaticCompareRemoteInput()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Base Library"),
                                "/home",
                                tr("JPEG Files (*.jpg *.jpeg)"),
                                &selectedFilter);
    QFileInfo Imageinfo(fileName);
    if (!fileName.isEmpty())
    {
        MtcManager::instance()->inputLibrary(fileName);
    }
}

void MainWindow::addFeature()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Add Feature"),
                                "/home",
                                tr("FEATURE Files(*.feature)"),
                                &selectedFilter);
//    QString fileName = QFileDialog::getOpenFileName(this,
//                                tr("Add Feature"),
//                                "/home",
//                                tr("FEATURE Files (*.*)"),
//                                &selectedFilter);
    QFileInfo Featureinfo(fileName);
    //qDebug() << "FEATURE size is " << Featureinfo.size();
    if (!fileName.isEmpty())
    {
        bool ok = false;
        QString featureId = InputDialog::getText(this, tr("Get Feature ID"),
                                             tr("Feature ID:"),&ok);
        if (ok && !featureId.isEmpty())
        {
            int index = featureId.indexOf("\\");
            if(index != -1)
            {
                QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
                return;
            }
            MtcManager::instance()->addFeature(featureId, fileName);
        }
        else
        {
            if(ok){
                QMessageBox::warning(this,"SenseEnginie Camera","Input image id failed");
            }
        }
    }
}

void MainWindow::queryFeature()
{
    bool ok = false;
    QString featureId = InputDialog::getText(this, tr("Get Feature ID"),
                                         tr("Feature ID:"),&ok);
    if (ok && !featureId.isEmpty())
    {
        int index = featureId.indexOf("\\");
        if(index != -1)
        {
            QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
            return;
        }
        MtcManager::instance()->queryFeature(featureId);
    }

}

void MainWindow::addBatch()
{
    AddBatchWidget *addBatchWidget = new AddBatchWidget(this);
    addBatchWidget->show();
}

void MainWindow::setFrame()
{    
    MtcManager::instance()->getFrame();
}

void MainWindow::getIrLight()
{
    MtcManager::instance()->MtcManager::GetIrLight();
}

void MainWindow::getLogConfig()
{
    MtcManager::instance()->MtcManager::GetLogConfig();
}

void MainWindow::setRecognizeCount()
{
//    int recognizeCount = 10,livingCount = 10;
//    setrecognizecount *setrecognizecountWidget = new setrecognizecount(this);
//    setrecognizecountWidget->initWidget("Set Recognize Count");
//    setrecognizecountWidget->show();
//    recognizeCount = setrecognizecountWidget->getRecognizeCount();
//    livingCount = setrecognizecountWidget->getLivingCount();
//    qDebug()<<"recognizeCount:"<<recognizeCount <<"livingCount:"<<livingCount;

    setrecognizecount *setrecognizecountWidget = new setrecognizecount(this);
    setrecognizecountWidget->initWidget("Set Recognize Count");
    connect(setrecognizecountWidget,&setrecognizecount::getCountSignal,this,&MainWindow::setRecognizeCountDispose);
    setrecognizecountWidget->show();

}

void MainWindow::setRecognizeCountDispose(int recognizeCount,int livingCount)
{
    //qDebug()<<"recognizeCount:"<<recognizeCount<<" livingCount:"<<livingCount;
    MtcManager::instance()->setRecognizeCount(recognizeCount,livingCount);
}

void MainWindow::getRecognizeCount()
{
    MtcManager::instance()->MtcManager::getRecognizeCount();
}

void MainWindow::setSwitchQRCode()
{
    this->m_showMsgDialog = true;
    QRCodeSwitchDialog *qrcodeSwitchDialog = new QRCodeSwitchDialog(this);
//        connect(irLightConfigDialog,&SettingWidget::applyCamera,this,&MainWindow::setCamera);
    qrcodeSwitchDialog->setQRCodeStatus(m_qrcodeStatus,m_qrcodeInterval);
    qrcodeSwitchDialog->show();
}

void MainWindow::qrCodeReportSlot(QString result)
{
   if(m_qrcode_file == nullptr)
   {
       QString fileDir = QString(QGuiApplication::applicationDirPath() + "/userData");
       QDir dir;
       if(!dir.exists(fileDir)){
            dir.mkpath(fileDir);
       }
       QString filePath = QString(fileDir +"/%1_qrcodeLog.txt").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd"));
       m_qrcode_file = new QFile();
       m_qrcode_file->setFileName(filePath);
       if(!QFile::exists(filePath))
       {
           m_qrcode_file->open(QIODevice::ReadWrite);
           m_qrcode_file->close();
       }
       //qDebug() << "file creat....";
       if(!m_qrcode_file->open(QIODevice::Append))
       {
           //qDebug() << "--- File open error! ---" << endl;
       }
   }
   if(m_qrcode_file != nullptr) {
       QTextStream stream(m_qrcode_file);
       stream << "[ " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz") << "] "<< result << "\r\n";
       m_qrcode_file->flush();
   }
    ui->msgLabel->setText(result);
    m_msgTimer.start(2000);
}

void MainWindow::getCpuTemperature()
{
    MtcManager::instance()->MtcManager::getCpuTemperature();
}

void MainWindow::getAESensorModel()
{
    MtcManager::instance()->MtcManager::getAESensorModel();
}

void MainWindow::getCpuTemperatureSlot(int result, int temp)
{
    QString title = "Cpu Temperature";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    if (result == 0)
    {
#ifdef __GNUC__
        msg = tr("Current Cpu temperature is %1 %2").arg(temp).arg("°C");
#else
        msg = tr("Current Cpu temperature is %1 %2").arg(temp).arg(QString::fromLocal8Bit("°C"));
#endif
    }
    else
    {
        level = ERROR_LEVEL;
        msg = tr("Get Cpu Temperature FAILED!");
    }

    this->showReceviceResult(title,msg,level);
}

void MainWindow::getAESensorModelSlot(int result, int sensorModel)
{
    QString title = "AE Sensor Model";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    if (result == 0)
    {
        //0x0: 未知sensor
        //0x2238 : sensor 2315E
        //0xCB14 : sensor 2335        
        if (sensorModel == 0x2238)
        {
            msg = tr("sensor 2315E");
        }
        else if (sensorModel == 0xCB14)
        {
            msg = tr("sensor 2335");
        }
        else
        {
            msg = tr("sensor unknown");
        }
    }
    else
    {
        level = ERROR_LEVEL;
        msg = tr("Get AE Sensor Model FAILED!");
    }

    this->showReceviceResult(title, msg, level);
}

void MainWindow::openTemlateUpdate()
{
    MtcManager::instance()->MtcManager::openTemlateUpdate();
}

void MainWindow::closeTemlateUpdate()
{
    MtcManager::instance()->MtcManager::closeTemlateUpdate();
}

void MainWindow::getTemlateUpdate()
{
    MtcManager::instance()->MtcManager::getTemlateUpdate();
}

void MainWindow::getAiConfig()
{
    MtcManager::instance()->MtcManager::GetAiConfig();
}

void MainWindow::getLightSensor()
{
    MtcManager::instance()->MtcManager::GetLightSensor();
}

void MainWindow::getVideoSizeResultSlot(int result, int width, int height)
{    
    if(result == 0)
    {
        /*
        0x00：W720p * H1280p
        0x01：W360p * H640p
        0x02：W720p * H720p(only Wukong)
        0x03：W720p * H640p(only Wukong)
        0x04：W720p * H480p(only Wukong)
        */
        bool ok = false;
        int  mode = 0;
        QStringList videoSizeList = { "W720p * H1280p","W360p * H640p" };
        QString videoSize = videoSizeList.at(mode);
        DeviceType deviceType = getDeviceType(); 
              
        if (((deviceType == DEVICE_M20) && (compareVersion(m_versionInfo,"V3.0.3") > 0)) || (deviceType == DEVICE_M20S))        
            videoSizeList.append(QList<QString>{"W720p * H720p", "W720p * H640p", "W720p * H480p"});                

        if (width == 360)
        {
            if (height == 640)      mode = 1; 
        }
        else if (width == 720)
        {
            if (height == 1280)     mode = 0;
            else if (height == 720) mode = 2;
            else if (height == 640) mode = 3;
            else if (height == 480) mode = 4;
        }        

        videoSize = QInputDialog::getItem(this, tr("Set Video Size"), tr("Mode:"),videoSizeList, mode, false, &ok);                
        if (ok)
        {
            mode = videoSizeList.indexOf(videoSize);
            if(mode >= 0)
            {
                MtcManager::instance()->MtcManager::setVideoSize(mode);
            }
            else
            {
                QString title = "Get Video Size";
                QString msg = "Get Video Size FAILED!";                
                RESULT_LEVEL level = ERROR_LEVEL;
                showReceviceResult(title, msg, level);
            }
        }
    }
    else
    {
        QString title = "Get Video Size";
        QString msg = "Get Video Size FAILED!";
        RESULT_LEVEL level = ERROR_LEVEL;
        showReceviceResult(title,msg,level);
    }
}

void MainWindow::setVideoSize()
{    
    MtcManager::instance()->MtcManager::getVideoSize();
}

void MainWindow::setScreenMode()
{
    int mode = 0;
    QStringList items;
    items << tr("Horizontal screen 0") << tr("Vertical screen 90") << tr("Horizontal screen 180") << tr("Vertical screen 270");

    bool ok;
    QString  screen = QInputDialog::getItem(this, tr("Set Screen Mode"),tr("Mode:"),
                                   items, 0, false, &ok);
    if (ok)
    {
        //qDebug() << "screen" << screen;
        if(screen == QString::fromLocal8Bit("Horizontal screen 0"))
        {
           //qDebug() << "mode screen" << screen;
           mode = 1;
        }
        else if(screen == QString::fromLocal8Bit("Vertical screen 90"))
        {
           //qDebug() << "mode screen" << screen;
           mode = 2;
        }
        else if(screen == QString::fromLocal8Bit("Horizontal screen 180"))
        {
           //qDebug() << "mode screen" << screen;
           mode = 3;
        }
        else if(screen == QString::fromLocal8Bit("Vertical screen 270"))
        {
           //qDebug() << "mode screen" << screen;
           mode = 4;
        }
        MtcManager::instance()->MtcManager::setScreenMode(mode);
    }
}

void MainWindow::setRotation()
{
    bool ok;
    int  angle = QInputDialog::getInt(this, tr("Set Rotation"),tr("Angle:"),
                                   0, 0, 270, 90, &ok);
    if (ok)
    {
        //qDebug() << "angle" << angle;
        MtcManager::instance()->MtcManager::setRotation(angle);
    }
}

void MainWindow::querySingle()
{
    bool ok;
    QString imageId = InputDialog::getText(this, tr("Get Image ID"),
                                         tr("Image ID:"),&ok);
    if (ok && !imageId.isEmpty())
    {
        int index = imageId.indexOf("\\");
        if(index != -1)
        {
            QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
            return;
        }
        MtcManager::instance()->queryFace(imageId);
    }
    else
    {
        if(ok)
        {
            QMessageBox::warning(this,"SenseEnginie Camera","Input image id failed");
        }
    }
}

void MainWindow::deleteOne()
{
    bool ok;
    QString imageId = InputDialog::getText(this, tr("Get Image ID"),
                                         tr("Image ID:"),&ok);
    if (ok && !imageId.isEmpty())
    {
        int index = imageId.indexOf("\\");
        if(index != -1)
        {
            QMessageBox::warning(this,this->windowTitle(),"Please check the validity of the input characters!");
            return;
        }
        MtcManager::instance()->deleteOne(imageId);
    }
    else
    {
        if(ok)
        {
            QMessageBox::warning(this,"SenseEnginie Camera","Input image id failed");
        }
    }
}

void MainWindow::deleteAll()
{
    if(QMessageBox::Yes == QMessageBox::question(this, "SenseEnginie Camera", "Don you want to delete all image?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
    {
        MtcManager::instance()->deleteAll();
    }else
    {
        return;
    }
}

void MainWindow::upgrade()
{
    UpgradeWidget *upgradWidget = new UpgradeWidget(this);
    connect(upgradWidget,&UpgradeWidget::upgradeSuccess,this,&MainWindow::upgradeSuccess);
    upgradWidget->show();
}

void MainWindow::showReceviceResult(QString title, QString msg, RESULT_LEVEL level)
{
    switch (level)
    {
    case ERROR_LEVEL:
    {
        QMessageBox::warning(this,title,msg);
        break;
    }
    case INFORM_LEVEL:
    {
        QMessageBox::information(this,title,msg);
        break;
    }
    }
}

void MainWindow::errorSlot(QString msg)
{
    QMessageBox::warning(this,"SenseEnginie Camera",msg);
}

void MainWindow::pingResult(bool result)
{
    if(result)
    {
        ui->statusBar->showMessage("check ping test is ok",3000);
    }
    else
    {
        this->showReceviceResult("Connect","Ping test failed, please re-plug device and try again",ERROR_LEVEL);
    }
}

void MainWindow::openFrameAutoResult(bool result)
{
    QString title = "open Frame Auto Report Result";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    if (result)
    {
        msg = tr("Open Frame Auto Report OK.");
    }else
    {
        level = ERROR_LEVEL;
        msg = tr("Open Frame Auto Report FAILED!");
    }

    this->showReceviceResult(title,msg,level);
}

void MainWindow::closeFrameAutoResult(bool result)
{
    QString title = "Close Frame Auto Report Result";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    if(result)
    {
        msg = tr("Close Frame Auto Report OK.");
    }else
    {
        level = ERROR_LEVEL;
        msg = tr("Close Frame Auto Report FAILED!");
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::setDeviceNoFlickerHzSignalResult(bool result)
{
    QString title = "Set Device Anti Flicker Result";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    if(result)
    {
//        msg = tr("Set Device Anti Flicker OK,when ae_mode is 0 in ai_config,it is Effective!");
        msg = tr("Set Device Anti Flicker OK!");
    }else
    {
        level = ERROR_LEVEL;
        msg = tr("Set Device Anti Flicker FAILED!");
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::getDeviceNoFlickerHzSignalResult(int result, int cameraId, int hz, int isEnable)
{
    if(result == 0)
    {
        AntiFlickerDialog *antiFlickerDialog = new AntiFlickerDialog(this);
        antiFlickerDialog->setAntiFlicker(cameraId, isEnable, hz);
        antiFlickerDialog->show();
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Anti Flicker FAILED!");
    }
}

/**
 * @brief MainWindow::checkModelM20
 * @param result 0:M20 10:M10 20:M20 pro
 * @param frameWidth
 * @param frameHeight
 */
void MainWindow::checkModelM20(DeviceType type, int frameWidth, int frameHeight)
{
    MtcManager::instance()->SwitchCameraRGB();
    ui->widget->setDeviceType(type);
    setDeviceType(type);
    switch (type) {
    case DEVICE_M10:
    {
        //        qDebug() << "checkModelM10 result is " << result;
        if(frameHeight != -1 && frameWidth != -1)
        {
            emit XFFmpeg::Get()->currentResolution(frameHeight,frameWidth);
        }
        //qDebug()<<"checkModelM10 frameHeight:"<< frameHeight<<"frameWidth:"<<frameWidth;
        ui->actionSet_Screen_Mode->setVisible(true);
        ui->actionSet_Rotation->setVisible(false);//M10 false

        ui->actionSwitch_Camera_IR->setVisible(false);
        ui->actionSwitch_Camera_RGB->setVisible(false);
        ui->actionOpen_IR->setVisible(false);
        ui->actionClose_IR->setVisible(false);
        ui->action_IR_Light_Config->setVisible(false);
        ui->actionGet_Sense_Value->setVisible(false);

        ui->actionIR_50HZ->setVisible(false);
        ui->actionIR_60HZ->setVisible(false);
        ui->actionIR_50HZ_2->setVisible(false);
        ui->actionIR_60HZ_2->setVisible(false);

        ui->actionSwitch_QRCode->setVisible(false);
        ui->actionGet_Cpu_Temp->setVisible(false);
        ui->actionGet_AE_Sensor->setVisible(false);
        break;
    }
    case DEVICE_M20:
    {
        //        qDebug() << "checkModelM20 result is " << result;
        ui->actionSet_Screen_Mode->setVisible(false);
        ui->actionSet_Rotation->setVisible(false);
        ui->actionSwitch_QRCode->setVisible(false);
        //ui->actionGet_Cpu_Temp->setVisible(false);
        break;
    }
    case DEVICE_M20S:
    {
        //        qDebug() << "checkModelM20S result is " << result;
        ui->actionSet_Screen_Mode->setVisible(false);
        ui->actionSet_Rotation->setVisible(false);
        MtcManager::instance()->setQRCodeSwitch(0,2);
        break;
    }
    case DEVICE_UNKNOW:
    default:
    {
        ui->actionSet_Screen_Mode->setVisible(false);
        ui->actionSet_Rotation->setVisible(false);

        ui->actionSwitch_Camera_IR->setVisible(false);
        ui->actionSwitch_Camera_RGB->setVisible(false);
        ui->actionOpen_IR->setVisible(false);
        ui->actionClose_IR->setVisible(false);
        ui->action_IR_Light_Config->setVisible(false);
        ui->actionGet_Sense_Value->setVisible(false);

        ui->actionMulti_Living->setVisible(false);//M20 has living validation
        ui->actionSingle_Living->setVisible(false);
        ui->actionOne_Multi_Living->setVisible(false);
        ui->actionOto_Single_Living->setVisible(false);

        ui->actionIR_50HZ->setVisible(false);
        ui->actionIR_60HZ->setVisible(false);
        ui->actionIR_50HZ_2->setVisible(false);
        ui->actionIR_60HZ_2->setVisible(false);

        ui->actionSwitch_QRCode->setVisible(false);
        ui->actionGet_Cpu_Temp->setVisible(false);
        ui->actionGet_AE_Sensor->setVisible(false);
    }
    }
}

void MainWindow::setDeviceType(DeviceType type)
{
    m_deviceType = type;
}

DeviceType MainWindow::getDeviceType()
{
    return m_deviceType;
}

void MainWindow::queryFaceResultSlot(int result, QString imageId)
{
    QString title = "QueryFace";
    QString msg;
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
        case 0:
            msg = tr("FaceID(%1) is exist").arg(imageId);
            break;
        case 1:
            msg = tr("FaceID(%1) is not exist").arg(imageId);
            break;
        case 2:
            msg = tr("Query faceId(%1) fail").arg(imageId);
            break;
        default:
            level = ERROR_LEVEL;
            msg = tr("Query face result failed!");
            break;
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::deleteImageResultSlot(int mode, int result)
{
    QString title = "DeleteImage";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (mode)
    {
        case MTC_DEL_IMAGE_ONE:
        {
            if(result == 0)
            {
                msg = tr("Delete image ok.");
            }
            else if(result == 1)
            {
                level = ERROR_LEVEL;
                msg = tr("Delete image failed,image id not find!");
            }
            else
            {
                level = ERROR_LEVEL;
                msg = tr("Delete image Timeout!");
            }
            break;
        }
        case MTC_DEL_IMAGE_ALL:
        {
            if(result != 0)
            {
                level = ERROR_LEVEL;
                msg = tr("Delete All image failed!");
            }
            else
            {
                msg = tr("Delete All image ok.");
            }
            break;
        }
        default:
            break;
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::inputLibraryResultSlot(int result, QString verifyinfo)
{
    switch (result)
    {
        case 0:
        {
            QMessageBox::information(this,"INPUT Library ok",QString("Face id:%1").arg(verifyinfo));
            break;
        }

        case 1:
        {
            QMessageBox::information(this,"INPUT Library failed","The picture without faces!");
            break;
        }
        case 2:
        {
            QMessageBox::information(this,"INPUT Library failed","The picture has multi-faces!");
            break;
        }
        case 3:
        {
            QMessageBox::information(this,"INPUT Library failed","The face area is too small!");
            break;
        }

        case 4:
        {
            QMessageBox::information(this,"INPUT Library failed","The Align score is too low!");
            break;
        }
        case 5:
        {
            QMessageBox::information(this,"INPUT Library failed","The picture has a side face!");
            break;
        }
        case 6:
        {
            QMessageBox::information(this,"INPUT Library failed","The face image is fuzzy!");
            break;
        }
        case 7:
        {
            QMessageBox::information(this,"INPUT Library failed","The picture faces is too far!");
            break;
        }
        case 8:
        {
            QMessageBox::information(this,"INPUT Library failed","init process failed!");
            break;
        }
        case 9:
        {
            QMessageBox::information(this,"INPUT Library failed","the function arg is invalid!");
            break;
        }
        case 10:
        {
            QMessageBox::information(this,"INPUT Library failed","the feature file is invalid!");
            break;
        }
        case 11:
        {
            QMessageBox::information(this,"INPUT Library failed","HW_AUTH init failed!");
            break;
        }
        case 12:
        {
            QMessageBox::information(this,"INPUT Library failed","the LICENSE add failed!");
            break;
        }
        case 13:
        {
            QMessageBox::information(this,"INPUT Library failed","the id is not exist!");
            break;
        }
        case 14:
        {            
            QMessageBox::information(this,"INPUT Library failed","please take off your mask!");
            break;
        }
        case 15:
        {
            QMessageBox::information(this,"INPUT Library failed","the Face database is full!");
            break;
        }
        case 16:
        {
            QMessageBox::information(this,"INPUT Library failed","the id is is invalid!");
            break;
        }
        case 17:
        {
            QMessageBox::information(this,"INPUT Library failed","the files saved failed!");
            break;
        }
        case 18:
        {
            QMessageBox::information(this,"INPUT Library failed","the file is not jpg!");
            break;
        }
        case 19:
        {
            QMessageBox::information(this,"INPUT Library failed","the RPC op-failed!");
            break;
        }
        case 20:
        {
            QMessageBox::information(this,"INPUT Library failed","the unknown TAG!");
            break;
        }
        case 160:
        {
            QMessageBox::information(this,"INPUT Library failed","the AI-SERVICE not ready!");
            break;
        }
        case -3:
        {
            QMessageBox::information(this,"INPUT Library failed","the picture data is error!");
            break;
        }
        case -10:
        {
            QMessageBox::information(this,"INPUT Library failed","the feature file is invalid!");
            break;
        }
        case 1001:
        {
            QMessageBox::information(this,"INPUT Library failed","the SDK_PROXY not ready!");
            break;
        }

        default:
        {
            //qDebug()<<"result:"<<result;
            //QMessageBox::information(this,"INPUT Library failed",QString("%1").arg(verifyinfo));
            if(verifyinfo.size())
            {
                QMessageBox::information(this,"INPUT Library failed",verifyinfo);
            }
            else
            {
                QMessageBox::information(this,"INPUT Library failed","The comparison is a failure!");
            }

            break;
        }
    }
}

void MainWindow::addImageResultSlot(int result)
{
    QString title = "ADD Image";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Add image ok.");
        break;
    }
    case 1:
    {
        msg = tr("Add image failed,The picture without faces!");
        break;
    }
    case 2:
    {
        msg = tr("Add image failed,The picture has multi-faces !");
        break;
    }
    case 3:
    {
        msg = tr("Add image failed,The face area is too small !");
        break;
    }
    case 4:
    {
        msg = tr("Add image failed,The Align score is too low!");
        break;
    }
    case 5:
    {
        msg = tr("Add image failed,The picture has a side face!");
        break;
    }
    case 6:
    {
        msg = tr("Add image failed,The face image is fuzzy !");
        break;
    }
    case 7:
    {
        msg = tr("Add image failed,The picture faces is too far!");
        break;
    }
    case 8:
    {
        msg = tr("Add image failed,init process failed!");
        break;
    }
    case 9:
    {
        msg = tr("Add image failed,the function arg is invalid !");
        break;
    }
    case 10:
    {
        msg = tr("Add image failed,the feature file is invalid !");
        break;
    }
    case 11:
    {
        msg = tr("Add image failed,HW_AUTH init failed!");
        break;
    }
    case 12:
    {
        msg = tr("Add image failed,the LICENSE add failed!");
        break;
    }
    case 13:
    {
        msg = tr("Add image failed,the id is not exist!");
        break;
    }
    case 14:
    {
        msg = tr("Add image failed,please take off your mask!");
        break;
    }
    case 15:
    {
        msg = tr("Add image failed,the Face database is full!");
        break;
    }
    case 16:
    {
        msg = tr("Add image failed,the id is is invalid!");
        break;
    }
    case 17:
    {
        msg = tr("Add image failed,the files saved failed");
        break;
    }
    case 18:
    {
        msg = tr("Add image failed,the file is not jpg!");
        break;
    }
    case 19:
    {
        msg = tr("Add image failed,the RPC op-failed!");
        break;
    }
    case 20:
    {
        msg = tr("Add image failed,the unknown TAG!");
        break;
    }
    case 160:
    {
        msg = tr("Add image failed,the AI-SERVICE not ready!");
        break;
    }
    case -3:
    {
        msg = tr("Add image failed,the picture data is error!");
        break;
    }
    case -10:
    {
        msg = tr("Add image failed,the feature file is invalid !");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Add image Timeout!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}
void MainWindow::startRecognizeResultSlot(int result)
{
    QString title = "Start Recognize";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Start recognize.");
        break;
    }
    case 1:
    {
        msg = tr("Start recognize not support.");
        break;
    }
    case 2:
    {
        msg = tr("Recognition is abnormal.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Start recognize Timeout!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}
void MainWindow::stopRecognizeResultSlot(int result)
{
    QString title = "Stop Recognize";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Stop recognize.");
        break;
    }
    case 1:
    {
        msg = tr("Pattern mismatch.");
        break;
    }
    case 2:
    {
        msg = tr("Recognition is Stop.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Stop recognize Timeout!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::queryRecognizeStatusResultSlot(int result)
{
    QString title = "Query Recognize Status";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
        case 0:
        {
            msg = tr("Recognize opening");
            break;
        }
        case 1:
        {
            msg = tr("Recognize closing");
            break;
        }
        default:
        {
            level = ERROR_LEVEL;
            msg = tr("Unknown recognize status");
            break;
        }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::onetooneRecognizeResultSlot(int result)
{
    QString title = "One to One Recognize";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
        case -1:
        {
            msg = tr("The picture is too large for more than 4M");
            break;
        }
        case 0:
        {
            msg = tr("One to One recognize starting success!.");
            break;
        }
        case 1:
        {
            msg = tr("The picture without faces!");
            break;
        }
        case 2:
        {
            msg = tr("The picture has multi-faces !");
            break;
        }
        case 3:
        {
            msg = tr("The face area is too small !");
            break;
        }
        case 4:
        {
            msg = tr("The Align score is too low!");
            break;
        }
        case 5:
        {
            msg = tr("The picture has a side face!");
            break;
        }
        case 6:
        {
            msg = tr("The face image is fuzzy !");
            break;
        }
        case 7:
        {
            msg = tr("The picture faces is too far!");
            break;
        }
        default:
        {
            level = ERROR_LEVEL;
            msg = tr("One to One recognize FAILED!");
            break;
        }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::queryFeatureResultSlot(int result)
{
    QString title = "Query Feature";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Query Feature OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Query Feature FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::getIDListResultSlot(int result)
{
    QString title = "Get IDList";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Get IDList OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Get IDList FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::openAutoReportResultSlot(int result)
{
    QString title = "Open Auto Report";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Open Auto Report is OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Open Auto Report FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::closeAutoReportResultSlot(int result)
{
    QString title = "Close Auto Report";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Close Auto Report is OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Close Auto Report FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::setFrameResultSlot(int result)
{
    QString title = "Set Frame";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
        case 0:
        {
            msg = tr("Set Frame OK.");
            break;
        }
        default:
        {
            level = ERROR_LEVEL;
            msg = tr("Set Frame FAILED!");
            break;
        }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::getFrameResultSlot(int result, int frame)
{
    if(result == 0 && frame >= 10 && frame <= 25)
    {
        bool ok;
        int inputValue = QInputDialog::getInt(this, tr("Set Frame"),tr("Frame:"),
                                       frame, 10, 25, 1,&ok);
        if (ok)
        {
            //qDebug() << "frame" << inputValue;
            MtcManager::instance()->setFrame(inputValue);
        }
    }
    else
    {
        this->showReceviceResult("Get Frame",tr("Get Frame FAILED!"),ERROR_LEVEL);
    }
}

void MainWindow::setIrLightResultSlot(int result)
{
    QString title = "Set IR Light";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set IR Light OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set IR Light FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::getIrLightResultSlot(int result, int lightValue, int closeTime)
{
    if(result == 0)
    {
        IrLightConfigDialog *irLightConfigDialog = new IrLightConfigDialog(this);
//        connect(irLightConfigDialog,&SettingWidget::applyCamera,this,&MainWindow::setCamera);
        //qDebug() << "[getIrLightResultSlot] " << closeTime;
        irLightConfigDialog->setConfig(lightValue,closeTime);
        irLightConfigDialog->show();
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get IR Light FAILED!");
    }
}

void MainWindow::setLogConfigResultSlot(int result)
{
    QString title = "Set Log Config";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set Log Config OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set Log Config FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::getLogConfigResultSlot(int result, int logLever ,int logTarget)
{
    if(result == 0)
    {
        LogSettingDialog *logSettingDialog = new LogSettingDialog(this);
        logSettingDialog->setLogConfig(logLever, logTarget);
        logSettingDialog->show();
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Log Config FAILED!");
    }
}

void MainWindow::setRecognizeCountResultSlot(int result)
{
    QString title = "Set Recognize Count";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set Recognize Count OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set Recognize Count FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}
void MainWindow::getRecognizeCountResultSlot(int result,int recognizeCount,int livingCount)
{
    if(result == 0)
    {
        QMessageBox::information(this,"SenseEnginie Camera",QString("recognizeCount:%1 livingCount:%2").arg(recognizeCount).arg(livingCount));
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Recognize Count FAILED!");
    }
}

void MainWindow::setQRCodeSwitchResultSlot(int mode, int interval, int result)
{
    QString title = "Set QRcode Switch";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set QRcode Switch OK.");
        this->m_qrcodeStatus = mode;
        this->m_qrcodeInterval = interval;
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set QRcode Switch FAILED!");
        break;
    }
    }
    if(m_showMsgDialog)
    {
        this->m_showMsgDialog = false;
        this->showReceviceResult(title,msg,level);
    }
}


void MainWindow::setTemlateUpdateResultSlot(int result,int temlateUpdateMode)
{
//    0x00: 关闭更新
//    0x01: 开启更新
    QString title;
    if(0 == temlateUpdateMode)
    {
        title = "Close Template Update";
    }
    else if(1 == temlateUpdateMode)
    {
        title = "Open Template Update";
    }
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = title + tr(" OK.Restart to take effect!");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = title + tr(" FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}
void MainWindow::getTemlateUpdateResultSlot(int result,int temlateUpdateMode)
{
    //0x00: Close the update.
    //0x01: Open the update.(default)
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Template Update failed!");
    }
    else
    {
        if(0 == temlateUpdateMode)
        {
            QMessageBox::information(this,"SenseEnginie Camera","Template Update is closing!");
        }else if(1 == temlateUpdateMode)
        {
            QMessageBox::information(this,"SenseEnginie Camera","Template Update is opening!");
        }

    }
}

void MainWindow::getLightSensorResultSlot(int result, int light)
{
    if(result == 0)
    {
        QMessageBox::information(this,"SenseEnginie Camera",QString("%1").arg(light));
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Light Sensor FAILED!");
    }
}

void MainWindow::setVideoSizeResultSlot(int result)
{
    //qDebug()<<"setVideoSizeResultSlot";
    QString title = "Set Video Size";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
        
    /*switch (result)
    {
    case 0:
    {
        msg = tr("Set Video Size OK");    
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set Video Size FAILED!");
        break;
    }
    }*/
    msg = tr("Set Video Size OK.");
    this->showReceviceResult(title,msg,level);
    disConnectionDev();
}

void MainWindow::setScreenModeResultSlot(int result)
{
    QString title = "Set Screen Mode";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set Screen Mode OK.It will restart and take effect.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set Screen Mode FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::setRotateAngleResultSlot(int result)
{
    QString title = "Set Rotate Angle";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set Rotate Angle OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set Rotate Angle FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::setAiConfigResultSlot(int result)
{
    QString title = "Set AI Config";
    QString msg = "";
    RESULT_LEVEL level = INFORM_LEVEL;
    switch (result)
    {
    case 0:
    {
        msg = tr("Set AI Config OK.");
        break;
    }
    default:
    {
        level = ERROR_LEVEL;
        msg = tr("Set AI Config FAILED!");
        break;
    }
    }
    this->showReceviceResult(title,msg,level);
}

void MainWindow::getAiConfigResultSlot(QString result)
{
    if(result.isEmpty())
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get AiConfig failed!");
    }
    else
    {
        bool ok;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(result.toUtf8());
        QString aiconfig = QInputDialog::getMultiLineText(this, tr("Set AI Config"),
                                             tr("AI Config:"),jsonDoc.toJson(QJsonDocument::Indented), &ok);
        if (ok && !aiconfig.isEmpty())
        {
            MtcManager::instance()->MtcManager::SetAiConfig(aiconfig.trimmed());
        }else
        {
            if(ok){
                QMessageBox::warning(this,"SenseEnginie Camera","Input aiconfig failed");
            }
        }
    }

}
void MainWindow::getVersionResultSlot(QString result)
{
    if (m_isGetVersion)
    {
        if (result.isEmpty())
        {
            QMessageBox::warning(this, "SenseEnginie Camera", "Get version failed!");
        }
        else
        {            
            QMessageBox::information(this, "SenseEnginie Camera", QString("%1").arg(result));
        }
    }
    else
    {
        m_isGetVersion = true;
        QStringList versionInfoList = result.split('\n');
        m_versionInfo = versionInfoList.at(0);
        QStringList softwareVersionList = m_versionInfo.split('=');
        if (softwareVersionList.size() == 2)
        {
            m_versionInfo = softwareVersionList.at(1).trimmed();
        }
    }
}

void MainWindow::getSingleRecognizeResultSlot(int result,QString recInfo)
{    
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Single Recognize failed!");
    }
    else
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(recInfo.toUtf8());
        QString aiconfig      = QInputDialog::getMultiLineText(this, tr("Get Single Recognize"),
                                tr("Recognize Info"),jsonDoc.toJson(QJsonDocument::Indented));
    }
}

void MainWindow::getSnResultSlot(QString result)
{
    if(result.isEmpty())
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get SN failed!");
    }
    else
    {
        QMessageBox::information(this,"SenseEnginie Camera",QString("%1").arg(result));
    }
}

void MainWindow::getModelResultSlot(QString result)
{
    if(result.isEmpty())
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Model failed!");
    }
    else
    {
        QMessageBox::information(this,"SenseEnginie Camera",QString("%1").arg(result));
    }
}

void MainWindow::getLibraryResult(int result)
{
    if(result >= 0)
    {
        QMessageBox::information(this,"SenseEnginie Camera",QString("%1").arg(result));
    }else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get Library failed!");
    }

}
void MainWindow::switchCameraRgbResultSlot(int result)
{    
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Switch Camera RGB failed!");
    }
    else
    {
        m_isOpenRGB = true;
        if(m_isInitCamera == false)
        {            
            //m_isInitCamera = true;
            MtcManager::instance()->MtcManager::OpenCamera(RGB_CAMERA);
            return;
        }
        QMessageBox::warning(this,"SenseEnginie Camera","Switch Camera RGB ok!");
        XFFmpeg::Get()->setFaceRectEnable(true);
    }
}
void MainWindow::switchCameraIrResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Switch Camera IR failed!");
    }
    else
    {
        m_isOpenRGB = false;
        QMessageBox::warning(this,"SenseEnginie Camera","Switch Camera IR ok!");
        XFFmpeg::Get()->setFaceRectEnable(false);
    }
}

void MainWindow::setCamera(QString cameraName, QString serialPort)
{
    m_serailStatus->clear();
    m_cameraStatus->clear();

    if(cameraName.isEmpty())
    {
        m_cameraStatus->setText(tr("Camera device not found!"));
        QMessageBox::warning(this,"SenseEnginie Camera","Camera device not found!");
        return;
    }
    XFFmpeg::Get()->Setup(MtcManager::instance()->GetDevHandle());
    MtcManager::instance()->openSerialPort(serialPort);

    m_cameraName = cameraName;
//    if(XFFmpeg::Get()->Open(cameraName) == 0)
//    {
//        qDebug() << "MainWindow::Camera open success!";
//        m_cameraStatus->setText(tr("Camera:%1").arg(cameraName));
//        XVideoThread::Get()->start();
//        //MtcManager::instance()->openSerialPort(serialPort);
//    }
//    else
//    {
//        qDebug() << "MainWindow::Camera open fail!";
//        m_cameraStatus->setText(tr("Camera open fail"));
//    }
}

void MainWindow::upgradeSuccess()
{
    disConnectionDev();
}

void MainWindow::reboot()
{
    MtcManager::instance()->Reboot(MTC_REBOOT);
}

void MainWindow::recovery()
{
    int mode = 0;
    QStringList items;
    items << tr("Delete config") << tr("Delete config and library");//Remove an option because both inputs delete the base library
    //items << tr("Delete config and library");
    bool ok;
    QString  recmode = QInputDialog::getItem(this, tr("Set Recovery Mode"),tr("Mode:"),
                                   items, 0, false, &ok);
    if (ok)
    {
        //qDebug() << "recmode" << recmode;
        if(recmode == QString::fromLocal8Bit("Delete config"))
        {
           //qDebug() << "mode " << recmode;
           mode = 1;
           //qDebug() << "mode " << mode;
        }
        else if(recmode == QString::fromLocal8Bit("Delete config and library"))
        {
           //qDebug() << "mode " << recmode;
           mode = 0;
           //qDebug() << "mode " << mode;
        }
        MtcManager::instance()->MtcManager::Recovery(mode);
    }
}
void MainWindow::userRebootWithDisk()
{
    MtcManager::instance()->Reboot(MTC_REBOOT_USER_DISK);
}

void MainWindow::userRebootWithoutDisk()
{
    MtcManager::instance()->Reboot(MTC_REBOOT_USER_NODISK);
}

void MainWindow::factoryRebootWithDisk()
{
    MtcManager::instance()->Reboot(MTC_REBOOT_FACT_DISK);
}

void MainWindow::factoryRebootWithoutDisk()
{
    MtcManager::instance()->Reboot(MTC_REBOOT_FACT_NODISK);
}


void MainWindow::setUmodeToEngin()
{
    MtcManager::instance()->SetUmodeToEngin();
}

void MainWindow::reset()
{
    MtcManager::instance()->Reset();
}

void MainWindow::setStreamFormat()
{
    MtcManager::instance()->SetStreamFormat();
}

void MainWindow::getModelAppVersion()
{
    MtcManager::instance()->GetModelAppVersion();
}

int MainWindow::compareVersion(QString version1, QString version2)
{            
    if (version1.at(0) != 'V' || version2.at(0) != 'V')
    {
        return -2;
    }
    
    QStringList version1List = version1.mid(1).split('.');
    QStringList version2List = version2.mid(1).split('.');
    if (version1List.size() != 3 || version2List.size() != 3)
    {
        return -3;
    }

    QList<int> version1NumList;    
    for(int i = 0;i < version1List.size();i++)
    {
        bool ok = true;
        int num = version1List.at(i).toInt(&ok);
        if (ok) version1NumList.append(num);
        else return -4;
    }    

    QList<int> version2NumList;
    for (int i = 0; i < version2List.size(); i++)
    {
        bool ok = true;
        int num = version2List.at(i).toInt(&ok);
        if (ok) version2NumList.append(num);
        else return -4;
    }
    
    for (int i = 0; i < version1NumList.size(); i++)
    {
        if (version1NumList.at(i) > version2NumList.at(i))
        {
            return 1;
        }
        else if (version1NumList.at(i) == version2NumList.at(i))
        {
            continue;
        }
        else if (version1NumList.at(i) < version2NumList.at(i))
        {
            return -1;
        }
    }

    return 0;
}

void MainWindow::rebootResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Reboot FAILED!");
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Reboot OK!");
        disConnectionDev();
    }
}

void MainWindow::setUmodeToEnginResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Set umode to engin failed!");
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Set umode to engin ok!");
        disConnectionDev();
    }
}

void MainWindow::resetResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Reset failed!");
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Reset ok!");
        disConnectionDev();
    }
}

void MainWindow::setStreamFormatResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Set stream format failed!");
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Set stream format ok!");
    }
}

void MainWindow::getModelAppVersionResultSlot(int result, QString modelAppVersion)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Get model app version failed!");
    }
    else
    {
        QMessageBox::information(this,"SenseEnginie Camera",QString("%1").arg(modelAppVersion));
    }
}

void MainWindow::recoveryResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Recovery FAILED!");
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","Recovery OK!");
        disConnectionDev();
    }
}

void MainWindow::opencameraResultSlot(int result)
{
    if(m_isInitCamera == false)
    {
        m_isInitCamera = true;
        if(XFFmpeg::Get()->Open(m_cameraName) == 0)
        {
            //qDebug() << "MainWindow::Camera open success!";
            m_cameraStatus->setText(tr("Camera:%1").arg(m_cameraName));
            XVideoThread::Get()->start();
            //MtcManager::instance()->openSerialPort(serialPort);
        }
        else
        {
            //qDebug() << "MainWindow::Camera open fail!";
            m_cameraStatus->setText(tr("Camera open fail"));
        }
        return;
    }

    switch(result)
    {
        case 0:
            QMessageBox::warning(this,"SenseEnginie Camera","opencamera OK!");
            break;
        case 1:
            QMessageBox::warning(this,"SenseEnginie Camera","camera is opening!");
            break;
        default:
            QMessageBox::warning(this,"SenseEnginie Camera","opencamera OK!");
    }
}

void MainWindow::closecameraResultSlot(int result)
{
    if(result != 0)
    {
        QMessageBox::warning(this,"SenseEnginie Camera","closecamera FAILED!");
    }
    else
    {
        QMessageBox::warning(this,"SenseEnginie Camera","closecamera OK!");
    }
}

void MainWindow::setActionEnable(bool isEnable)
{
    ui->actionSetting->setEnabled(!isEnable);
    ui->actionDisConnection->setEnabled(isEnable);
    ui->actionAdd_base_library_by_Camera->setEnabled(isEnable);
    ui->actionAdd_base_library_by_Camera_return->setEnabled(isEnable);
    ui->actionAdd_base_library->setEnabled(isEnable);
    ui->actionBatch_addition->setEnabled(isEnable);
    ui->actionQuery_Single->setEnabled(isEnable);
    ui->actionDelete->setEnabled(isEnable);
    ui->actionDelete_All->setEnabled(isEnable);
    ui->actionupgrade->setEnabled(isEnable);
    ui->actionStart_Recognize->setEnabled(isEnable);
    ui->actionStop_Recognize->setEnabled(isEnable);
    //ui->actionQuery_Recognize_Status->setEnabled(isEnable);
    ui->actionGet_Version->setEnabled(isEnable);
    ui->actionGet_Cpu_Temp->setEnabled(isEnable);
    ui->actionGet_AE_Sensor->setEnabled(isEnable);
    ui->actionSwitch_Camera_RGB->setEnabled(isEnable);
    ui->actionSwitch_Camera_IR->setEnabled(isEnable);
    ui->actionGet_Library_2->setEnabled(isEnable);
    //ui->actionInput_library->setEnabled(isEnable);
    ui->menuStatic_Compare->setEnabled(isEnable);
    ui->menuOne_to_Num_Reomode->setEnabled(isEnable);
    ui->menuOne_to_One_Reomode->setEnabled(isEnable);
    ui->actionAdd_feature->setEnabled(isEnable);
    ui->actionGet_feature->setEnabled(isEnable);
    ui->actionSet_Frame->setEnabled(isEnable);
    ui->action_IR_Light_Config->setEnabled(isEnable);
    ui->actionSet_AI_Config->setEnabled(isEnable);
    ui->actionGet_Sense_Value->setEnabled(isEnable);
    ui->menuReboot->setEnabled(isEnable);
    ui->actionGet_SN->setEnabled(isEnable);
    ui->actionGet_Model->setEnabled(isEnable);
    ui->actionSet_Video_Size->setEnabled(isEnable);
    ui->actionSet_Screen_Mode->setEnabled(isEnable);
    ui->actionSet_Rotation->setEnabled(isEnable);
    ui->actionRecovery->setEnabled(isEnable);
    ui->action_AI_Config->setEnabled(isEnable);
    ui->actionSet_Recognize_Count->setEnabled(isEnable);
    ui->actionGet_Recognize_Count->setEnabled(isEnable);
    ui->menuSet_Template_Update->setEnabled(isEnable);
    ui->actionGet_Template_Update->setEnabled(isEnable);
    ui->menuOpen_Camera->setEnabled(isEnable);
    ui->menuClose_Camera->setEnabled(isEnable);
    ui->actionGet_ID_List->setEnabled(isEnable);
    ui->actionSwitch_QRCode->setEnabled(isEnable);
    ui->actionGet_Single_Recognize->setEnabled(isEnable);

    ui->menuAuto_Report->setEnabled(isEnable);
//    ui->menuAuto_Report->menuAction()->setVisible(false);//Hidden menuAuto
//    ui->menuAuto_Report->hide();

    ui->menuSetIsMirrored->setEnabled(isEnable);
    ui->menuFrame_Report->setEnabled(isEnable);    
    ui->menuSet_Anti_Flicker->setEnabled(isEnable);

    ui->actionSetUmodeToEngin->setEnabled(isEnable);
    ui->actionReset->setEnabled(isEnable);
    //ui->actionSet_Stream_Format->setEnabled(isEnable);
    ui->actionGet_Model_App_Version->setEnabled(isEnable);
}

MainWindow::~MainWindow()
{    
    closeAutoReport();
    disConnectionDev();
    //delete XVideoThread::Get();
    delete MtcManager::instance();

    delete ui;

    if(m_serailStatus != nullptr)
    {
        delete m_serailStatus;
        m_serailStatus = nullptr;
    }

    if(m_cameraStatus != nullptr)
    {
        delete m_cameraStatus;
        m_cameraStatus = nullptr;
    }

    if(m_devReopenTip != nullptr)
    {
        delete m_devReopenTip;
        m_devReopenTip = nullptr;
    }

    //qDebug() << "MainWindow::~MainWindow in";
}
