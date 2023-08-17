#include "mtcmanager.h"
#include <QDebug>

QMutex MtcManager::s_mutex;
MtcManager *MtcManager::m_pInstance = nullptr;
MtcManager *MtcManager::instance()
{
    if(!m_pInstance)//First recognition
    {
        QMutexLocker locker(&s_mutex);//Add mutex.

        if(!m_pInstance){//Second recognition
            m_pInstance = new MtcManager();
            m_pInstance->m_mtcInterface = new MtcInterface();
        }
    }

    return m_pInstance;
}

MtcManager::MtcManager(QObject *parent) : QObject(parent)
{

}

MtcManager::~MtcManager()
{
    this->closePort();

    if(m_thread != nullptr)
    {        
        //qDebug() << "MtcManager m_mtcInterface thread exit |  m_mtcInterface ThreadId is " << m_thread->currentThreadId();
        m_thread->quit();
        m_thread->wait(3000);
        m_thread->deleteLater();
        m_thread = nullptr;
    }

    if(m_mtcInterface)
    {
        delete m_mtcInterface;
        m_mtcInterface = nullptr;
    }
}

void MtcManager::closePort()
{
    if(m_mtcInterface != nullptr)
    {
        emit this->closeSerialPort();
    }    
}

void MtcManager::open()
{
    if(m_mtcInterface == nullptr)
    {
        m_mtcInterface = new MtcInterface();
        mtcInterface_connected = false;
    }

    if(m_thread == nullptr)
    {
        m_thread = new QThread();
    }else
    {
        m_thread->quit();
    }


    if(!mtcInterface_connected)
    {
        connect(this,&MtcManager::initDevSingnal,m_mtcInterface,&MtcInterface::initDev);
        connect(this,&MtcManager::setPortNameSingnal,m_mtcInterface,&MtcInterface::setPortName);

        connect(this,&MtcManager::startOpenPort,m_mtcInterface,&MtcInterface::openSerialPort);
        connect(this,&MtcManager::closeSerialPort,m_mtcInterface,&MtcInterface::closeSerialPort);

        connect(this,&MtcManager::mtcOpenAutoUploadFaceInfoInFrameSingnal,m_mtcInterface,&MtcInterface::mtc_openAutoUploadFaceInfoInFrameResult);
        connect(this,&MtcManager::mtcCloseAutoUploadFaceInfoInFrameSingnal,m_mtcInterface,&MtcInterface::mtc_closeAutoUploadFaceInfoInFrameResult);
        connect(this,&MtcManager::mtcPintTestSignal,m_mtcInterface,&MtcInterface::mtc_ping);
        connect(this,&MtcManager::check_modelSignal,m_mtcInterface,&MtcInterface::check_model);
        connect(this,&MtcManager::queryFaceSignal,m_mtcInterface,&MtcInterface::mtc_query_face);
        connect(this,&MtcManager::deleteImageSignal,m_mtcInterface,&MtcInterface::_deleteImage);
        connect(this,&MtcManager::addImageSignal,m_mtcInterface,&MtcInterface::_addImage);
        connect(this,&MtcManager::inputLibrarySignal,m_mtcInterface,&MtcInterface::inputLibrary);
        connect(this,&MtcManager::addFeatureSignal,m_mtcInterface,&MtcInterface::mtcAddFeature);
        connect(this,&MtcManager::queryFeatureSignal,m_mtcInterface,&MtcInterface::mtc_query_feature);
        connect(this,&MtcManager::setFrameSignal,m_mtcInterface,&MtcInterface::mtc_set_frame);
        connect(this,&MtcManager::getFrameSignal,m_mtcInterface,&MtcInterface::mtc_get_frame);

        connect(this,&MtcManager::setIrLightSignal,m_mtcInterface,&MtcInterface::mtc_set_irlight);
        connect(this,&MtcManager::getIrLightSignal,m_mtcInterface,&MtcInterface::mtc_get_irlight);
        connect(this,&MtcManager::setLogConfigSignal,m_mtcInterface,&MtcInterface::mtc_set_logconfig);
        connect(this,&MtcManager::getLogConfigSignal,m_mtcInterface,&MtcInterface::mtc_get_logconfig);
        connect(this,&MtcManager::setRecognizeCountSingnal,m_mtcInterface,&MtcInterface::mtc_set_RecognizeCount);
        connect(this,&MtcManager::getRecognizeCountSingnal,m_mtcInterface,&MtcInterface::mtc_get_RecognizeCount);
        connect(this,&MtcManager::setQRCodeSwitchSingnal,m_mtcInterface,&MtcInterface::mtc_set_QRCodeSwitch);
        connect(this,&MtcManager::getCpuTemperatureSignal,m_mtcInterface,&MtcInterface::mtc_get_cpuTemperature);
        connect(this, &MtcManager::getAESensorModelSignal, m_mtcInterface, &MtcInterface::mtc_get_aesensormodel);

        connect(this,&MtcManager::openTemlateUpdateSingnal,m_mtcInterface,&MtcInterface::mtc_open_TemlateUpdate);
        connect(this,&MtcManager::closeTemlateUpdateSingnal,m_mtcInterface,&MtcInterface::mtc_close_TemlateUpdate);
        connect(this,&MtcManager::getTemlateUpdateSingnal,m_mtcInterface,&MtcInterface::mtc_get_TemlateUpdate);

        connect(this,&MtcManager::setAiConfigSignal,m_mtcInterface,&MtcInterface::mtc_set_aiconfig);
        connect(this,&MtcManager::getAiConfigSignal,m_mtcInterface,&MtcInterface::mtc_get_aiconfig);
        connect(this,&MtcManager::getLightSensorSignal,m_mtcInterface,&MtcInterface::mtc_get_lightsensor);
        connect(this,&MtcManager::setVideoSizeSignal,m_mtcInterface,&MtcInterface::mtc_set_videosize);
        connect(this,&MtcManager::getVideoSizeSignal,m_mtcInterface,&MtcInterface::mtc_get_videosize);
        connect(this,&MtcManager::setScreenModeSignal,m_mtcInterface,&MtcInterface::mtc_set_screenmode);
        connect(this,&MtcManager::setRotationSignal,m_mtcInterface,&MtcInterface::mtc_set_rotation);
        connect(this,&MtcManager::addImageBatchSignal,m_mtcInterface,&MtcInterface::addImageBatch);
        connect(this,&MtcManager::openCameraSignal,m_mtcInterface,&MtcInterface::openCamera);
        connect(this,&MtcManager::closeCameraSignal,m_mtcInterface,&MtcInterface::closeCamera);

        connect(this,&MtcManager::upgradeSignal,m_mtcInterface,&MtcInterface::upgradeSlots);
        connect(this,&MtcManager::mtcStartRecognizeSignal,m_mtcInterface,&MtcInterface::startRecognize);
        connect(this,&MtcManager::mtcStopRecognizeSignal,m_mtcInterface,&MtcInterface::stopRecognize);
        connect(this,&MtcManager::mtcQueryRecognizeStatusSignal,m_mtcInterface,&MtcInterface::QueryRecognizeStatus);
        connect(this,&MtcManager::onetonumReomodeSignal,m_mtcInterface,&MtcInterface::mtcStartOnetoNumRecognize);
        connect(this,&MtcManager::startOnetooneSignal,m_mtcInterface,&MtcInterface::mtc_start_onetoone_recognize);
        connect(this,&MtcManager::mtcGetVersionSignal,m_mtcInterface,&MtcInterface::getVersion);
        connect(this,&MtcManager::mtcGetSingleRecognizeSignal,m_mtcInterface,&MtcInterface::getSingleRecognize);
        connect(this,&MtcManager::mtcgetSNSignal,m_mtcInterface,&MtcInterface::getSN);
        connect(this,&MtcManager::mtcgetModelSignal,m_mtcInterface,&MtcInterface::getModel);
        connect(this,&MtcManager::mtcGetLibrarySignal,m_mtcInterface,&MtcInterface::getLibrary);
        connect(this,&MtcManager::mtcSwitchCameraRGBSignal,m_mtcInterface,&MtcInterface::switchCameraRGB);
        connect(this,&MtcManager::mtcSwitchCameraIRSignal,m_mtcInterface,&MtcInterface::switchCameraIR);
        connect(this,&MtcManager::RebootSignal,m_mtcInterface,&MtcInterface::mtc_reboot);
        connect(this,&MtcManager::RecoverySignal,m_mtcInterface,&MtcInterface::mtc_recovery);
        connect(this,&MtcManager::OpenCameraSignal,m_mtcInterface,&MtcInterface::mtc_opencamera);
        connect(this,&MtcManager::CloseCameraSignal,m_mtcInterface,&MtcInterface::mtc_closecamera);
        connect(this,&MtcManager::GetIDListSignal,m_mtcInterface,&MtcInterface::mtc_getidlist);
        connect(this,&MtcManager::OpenAutoReportSignal,m_mtcInterface,&MtcInterface::mtc_openAutoReport);
        connect(this,&MtcManager::CloseAutoReportSignal,m_mtcInterface,&MtcInterface::mtc_closeAutoReport);

        connect(this,&MtcManager::OpenFrameReportSignal,m_mtcInterface,&MtcInterface::mtc_openAutoUploadFaceInfoInFrameResult);
        connect(this,&MtcManager::CloseFrameReportSignal,m_mtcInterface,&MtcInterface::mtc_closeAutoUploadFaceInfoInFrameResult);

        connect(this,&MtcManager::antiFlickerSignal,m_mtcInterface,&MtcInterface::mtc_antiFlickerSignalResult);
        connect(this,&MtcManager::GetAntiFlickerSignal,m_mtcInterface,&MtcInterface::mtc_getAntiFlickerSignalResult);

        connect(this,&MtcManager::SetUmodeToEnginSignal,m_mtcInterface,&MtcInterface::mtc_setUmodeToEngin);
        connect(this,&MtcManager::ResetSignal,m_mtcInterface,&MtcInterface::mtc_reset);
        connect(this,&MtcManager::SetStreamFormatSignal,m_mtcInterface,&MtcInterface::mtc_setStreamFormat);
        connect(this,&MtcManager::GetModelAppVersionSignal,m_mtcInterface,&MtcInterface::mtc_getModelAppVersion);

        connect(m_mtcInterface,&MtcInterface::portStatusMsgSignal,this,&MtcManager::portStatusMsgSignal);
        connect(m_mtcInterface,&MtcInterface::errorSignal,this,&MtcManager::errorSignal);

        connect(m_mtcInterface,&MtcInterface::openFrameAutoResult,this,&MtcManager::openFrameAutoResult);
        connect(m_mtcInterface,&MtcInterface::closeFrameAutoResult,this,&MtcManager::closeFrameAutoResult);
        connect(m_mtcInterface,&MtcInterface::setDeviceNoFlickerHzSignal,this,&MtcManager::setDeviceNoFlickerHzSignal);
        connect(m_mtcInterface,&MtcInterface::getDeviceNoFlickerHzSignal,this,&MtcManager::getDeviceNoFlickerHzSignal);


        connect(m_mtcInterface,&MtcInterface::pingResult,this,&MtcManager::pingResult);
        connect(m_mtcInterface,&MtcInterface::checkModelM20,this,&MtcManager::checkModelM20);
        connect(m_mtcInterface,&MtcInterface::queryFaceResult,this,&MtcManager::queryFaceResult);
        connect(m_mtcInterface,&MtcInterface::deleteImageResult,this,&MtcManager::deleteImageResult);
        connect(m_mtcInterface,&MtcInterface::addImageResult,this,&MtcManager::addImageResult);
        connect(m_mtcInterface,&MtcInterface::inputLibraryResult,this,&MtcManager::inputLibraryResult);
        connect(m_mtcInterface,&MtcInterface::addImageBatchStatus,this,&MtcManager::addImageBatchResultSignal);
        connect(m_mtcInterface,&MtcInterface::uploadPackageProgress,this,&MtcManager::uploadPackageProgress);
        connect(m_mtcInterface,&MtcInterface::setUploadRange,this,&MtcManager::setUploadRange);
        connect(m_mtcInterface,&MtcInterface::uploadStatus,this,&MtcManager::uploadStatus);
        connect(m_mtcInterface,&MtcInterface::startRecognizeResult,this,&MtcManager::startRecognizeResult);
        connect(m_mtcInterface,&MtcInterface::stopRecognizeResult,this,&MtcManager::stopRecognizeResult);
        connect(m_mtcInterface,&MtcInterface::queryRecognizeStatusResult,this,&MtcManager::queryRecognizeStatusResult);
        connect(m_mtcInterface,&MtcInterface::onetooneRecognizeResult,this,&MtcManager::onetooneRecognizeResult);
        connect(m_mtcInterface,&MtcInterface::queryFeatureResult,this,&MtcManager::queryFeatureResult);
        connect(m_mtcInterface,&MtcInterface::getIDListResult,this,&MtcManager::getIDListResult);
        connect(m_mtcInterface,&MtcInterface::openAutoReportResult,this,&MtcManager::openAutoReportResult);
        connect(m_mtcInterface,&MtcInterface::closeAutoReportResult,this,&MtcManager::closeAutoReportResult);
        connect(m_mtcInterface,&MtcInterface::setFrameResult,this,&MtcManager::setFrameResult);
        connect(m_mtcInterface,&MtcInterface::getFrameResult,this,&MtcManager::getFrameResult);
        connect(m_mtcInterface,&MtcInterface::setIrLightResult,this,&MtcManager::setIrLightResult);
        connect(m_mtcInterface,&MtcInterface::getIrLightResult,this,&MtcManager::getIrLightResult);
        connect(m_mtcInterface,&MtcInterface::setLogConfigResult,this,&MtcManager::setLogConfigResult);
        connect(m_mtcInterface,&MtcInterface::getLogConfigResult,this,&MtcManager::getLogConfigResult);
        connect(m_mtcInterface,&MtcInterface::setQRCodeSwitchResult,this,&MtcManager::setQRCodeSwitchResult);
        connect(m_mtcInterface,&MtcInterface::qrCodeReportSignal,this,&MtcManager::qrCodeReportSignal);
        connect(m_mtcInterface,&MtcInterface::getCpuTemperatureResult,this,&MtcManager::getCpuTemperatureResult);
        connect(m_mtcInterface,&MtcInterface::getAESensorModelResult, this, &MtcManager::getAESensorModelResult);

        connect(m_mtcInterface,&MtcInterface::setRecognizeCountResult,this,&MtcManager::setRecognizeCountResult);
        connect(m_mtcInterface,&MtcInterface::getRecognizeCountResult,this,&MtcManager::getRecognizeCountResult);

        connect(m_mtcInterface,&MtcInterface::setTemlateUpdateResult,this,&MtcManager::setTemlateUpdateResult);
        connect(m_mtcInterface,&MtcInterface::getTemlateUpdateResult,this,&MtcManager::getTemlateUpdateResult);

        connect(m_mtcInterface,&MtcInterface::getLightSensorResult,this,&MtcManager::getLightSensorResult);
        connect(m_mtcInterface,&MtcInterface::setVideoSizeResult,this,&MtcManager::setVideoSizeResult);
        connect(m_mtcInterface,&MtcInterface::getVideoSizeResult,this,&MtcManager::getVideoSizeResult);
        connect(m_mtcInterface,&MtcInterface::setScreenModeResult,this,&MtcManager::setScreenModeResult);
        connect(m_mtcInterface,&MtcInterface::setRotateAngleResult,this,&MtcManager::setRotateAngleResult);
        connect(m_mtcInterface,&MtcInterface::setAiConfigResult,this,&MtcManager::setAiConfigResult);
        connect(m_mtcInterface,&MtcInterface::getAiconfigResult,this,&MtcManager::getAiConfigResult);
        connect(m_mtcInterface,&MtcInterface::getVersionResult,this,&MtcManager::getVersionResult);
        connect(m_mtcInterface,&MtcInterface::getSingleRecognizeResult,this,&MtcManager::getSingleRecognizeResult);
        connect(m_mtcInterface,&MtcInterface::getSnResult,this,&MtcManager::getSnResult);
        connect(m_mtcInterface,&MtcInterface::getModelResult,this,&MtcManager::getModelResult);
        connect(m_mtcInterface,&MtcInterface::getLibraryResult,this,&MtcManager::getLibraryResult);
        connect(m_mtcInterface,&MtcInterface::switchCameraRgbResult,this,&MtcManager::switchCameraRgbResult);
        connect(m_mtcInterface,&MtcInterface::switchCameraIrResult,this,&MtcManager::switchCameraIrResult);
        connect(m_mtcInterface,&MtcInterface::rebootResult,this,&MtcManager::rebootResult);
        connect(m_mtcInterface,&MtcInterface::recoveryResult,this,&MtcManager::recoveryResult);
        connect(m_mtcInterface,&MtcInterface::opencameraResult,this,&MtcManager::opencameraResult);
        connect(m_mtcInterface,&MtcInterface::closecameraResult,this,&MtcManager::closecameraResult);

        connect(m_mtcInterface,&MtcInterface::setUmodeToEnginResult,this,&MtcManager::setUmodeToEnginResult);
        connect(m_mtcInterface,&MtcInterface::resetResult,this,&MtcManager::resetResult);
        connect(m_mtcInterface,&MtcInterface::setStreamFormatResult,this,&MtcManager::setStreamFormatResult);
        connect(m_mtcInterface,&MtcInterface::getModelAppVersionResult,this,&MtcManager::getModelAppVersionResult);
    }    
    m_mtcInterface->moveToThread(m_thread);
    m_thread->start();
    //qDebug() << "MtcManager m_mtcInterface thread start |  m_mtcInterface ThreadId is " << m_thread->currentThreadId();
    emit initDevSingnal();
}

void MtcManager::openSerialPort(const QString &portName)
{
    //qDebug() << "MtcManager::openSerialPort: " << portName;
    mtcInterface_connected = true;
    emit setPortNameSingnal(portName);
    emit startOpenPort();
}

void MtcManager::mtcOpenAutoUploadFaceInfoInFrame()
{
    emit mtcOpenAutoUploadFaceInfoInFrameSingnal();
}

void MtcManager::mtcCloseAutoUploadFaceInfoInFrame()
{
    emit mtcCloseAutoUploadFaceInfoInFrameSingnal();
}

void MtcManager::mtcPingTest(QString data)
{
    m_pingData = data;
    emit mtcPintTestSignal(data);
}

void MtcManager::mtcCheckModel()
{
    emit check_modelSignal();
}

void MtcManager::queryFace(QString imageId)
{
    emit this->queryFaceSignal(imageId);
}

void MtcManager::deleteAll()
{
    emit this->deleteImageSignal(MTC_DEL_IMAGE_ALL,"");
}

void MtcManager::deleteOne(QString imageId)
{
    emit this->deleteImageSignal(MTC_DEL_IMAGE_ONE,imageId);
}

void MtcManager::addBaseLibraryByCamera(QString imageId)
{
    emit this->addImageSignal(MTC_ADD_IMAGE_LOCAL_CAPTURE,imageId,"");
}

void MtcManager::addBaseLibraryByCameraReturn(QString imageId)
{
     emit this->addImageSignal(MTC_ADD_IMAGE_CAPTURE_RETURN,imageId,"");
}

void MtcManager::addImageFile(QString imageId, QString filePath)
{
    emit this->addImageSignal(MTC_ADD_IMAGE_REMOTE_INPUT,imageId,filePath);
}

void MtcManager::inputLibrary(QString filePath)
{
    if(filePath.isEmpty())
    {
        emit this->inputLibrarySignal(MTC_ADD_IMAGE_LOCAL_CAPTURE,filePath);
    }
    else
    {
        emit this->inputLibrarySignal(MTC_ADD_IMAGE_REMOTE_INPUT,filePath);
    }
}

void MtcManager::addFeature(QString imageId, QString filePath)
{
    emit this->addFeatureSignal(imageId, filePath);
}

void MtcManager::queryFeature(QString featureId)
{
    emit this->queryFeatureSignal(featureId);
}

void MtcManager::setFrame(int frame)
{
    emit this->setFrameSignal(frame);
}

void MtcManager::getFrame()
{
    emit this->getFrameSignal();
}

void MtcManager::setIrLight(int irlight, int closeTime)
{
    emit this->setIrLightSignal(irlight,closeTime);
}

void MtcManager::GetIrLight()
{
    emit this->getIrLightSignal();
}

void MtcManager::setLogConfig(int logLever ,int logTarget)
{
    emit this->setLogConfigSignal(logLever ,logTarget);
}

void MtcManager::GetLogConfig()
{
    emit this->getLogConfigSignal();
}

void MtcManager::setRecognizeCount(int recognizeCount, int livingCount)
{
    emit this->setRecognizeCountSingnal(recognizeCount,livingCount);
}

void MtcManager::getRecognizeCount()
{
    emit this->getRecognizeCountSingnal();
}

void MtcManager::setQRCodeSwitch(int mode, int interval)
{
    emit this->setQRCodeSwitchSingnal(mode,interval);
}

void MtcManager::getCpuTemperature()
{
    emit this->getCpuTemperatureSignal();
}

void MtcManager::getAESensorModel()
{
    emit this->getAESensorModelSignal();
}

void MtcManager::openTemlateUpdate()
{
    emit this->openTemlateUpdateSingnal();
}

void MtcManager::closeTemlateUpdate()
{
    emit this->closeTemlateUpdateSingnal();
}

void MtcManager::getTemlateUpdate()
{
    emit this->getTemlateUpdateSingnal();
}


void MtcManager::GetLightSensor()
{
    emit this->getLightSensorSignal();
}

void MtcManager::setVideoSize(int size)
{
    emit this->setVideoSizeSignal(size);
}

void MtcManager::getVideoSize()
{
    emit this->getVideoSizeSignal();
}

void MtcManager::setScreenMode(int mode)
{
    emit this->setScreenModeSignal(mode);
}

void MtcManager::setRotation(int angle)
{
    emit this->setRotationSignal(angle);
}

void MtcManager::SetAiConfig(QString aiconfig)
{
    emit this->setAiConfigSignal(aiconfig);
}

void MtcManager::GetAiConfig()
{
    emit this->getAiConfigSignal();
}
void MtcManager::addImageBatch(QString fileName)
{
    emit this->addImageBatchSignal(fileName);
}

void MtcManager::closeCamera()
{
    emit this->closeCameraSignal();
}

void MtcManager::openCamera()
{
    emit this->openCameraSignal();
}

void MtcManager::cancelAddImageBatch()
{
    MtcInterface::cancelAddImageBatch();
}

void MtcManager::cancelUpgradeBatch()
{
    MtcInterface::cancelUpgradeBatch();
}

void MtcManager::upgrade(QString filePath)
{
    emit this->upgradeSignal(filePath);
}

void MtcManager::StartRecognize(int mode)
{
    emit this->mtcStartRecognizeSignal(mode);
}

void MtcManager::StopRecognize(int mode)
{
    emit this->mtcStopRecognizeSignal(mode);
}

void MtcManager::QueryRecognizeStatus()
{
    emit this->mtcQueryRecognizeStatusSignal();
}

void MtcManager::onetonumReomode(int reomode, int mulmode)
{
    emit this->onetonumReomodeSignal(reomode, mulmode);
}
void MtcManager::onetooneFile(int reomode, int mulmode, QString filePath)
{
    emit this->startOnetooneSignal(reomode, mulmode, filePath);
}
void MtcManager::GetVersion()
{
    emit this->mtcGetVersionSignal();
}

void MtcManager::getSingleRecognize()
{
    emit this->mtcGetSingleRecognizeSignal();
}

void MtcManager::getSN()
{
    emit this->mtcgetSNSignal();
}

void MtcManager::getModel()
{
    emit this->mtcgetModelSignal();
}
void MtcManager::GetLibrary()
{
    emit this->mtcGetLibrarySignal();
}

void MtcManager::SwitchCameraRGB()
{
    emit this->mtcSwitchCameraRGBSignal();
}
void MtcManager::SwitchCameraIR()
{
    emit this->mtcSwitchCameraIRSignal();
}

void MtcManager::OpenCamera(int mode)
{
    emit this->OpenCameraSignal(mode);
}

void MtcManager::CloseCamera(int mode)
{
    emit this->CloseCameraSignal(mode);
}

void MtcManager::GetIDList()
{
    emit this->GetIDListSignal();
}

void MtcManager::OpenAutoReport(int imageMode)
{
    emit this->OpenAutoReportSignal(imageMode);
}

void MtcManager::CloseAutoReport()
{
    emit this->CloseAutoReportSignal();
}

void MtcManager::OpenFrameReport()
{
    emit this->OpenFrameReportSignal();
}

void MtcManager::CloseFrameReport()
{
    emit this->CloseFrameReportSignal();
}

void MtcManager::antiFlicker(int isEnable,int RGBIR,int HZ)
{
    emit this->antiFlickerSignal(isEnable, RGBIR, HZ);
}

void MtcManager::GetAntiFlicker(int RGBIR)
{
    emit this->GetAntiFlickerSignal(RGBIR);
}

void MtcManager::Reboot(int mode)
{
    emit this->RebootSignal(mode);
}

void MtcManager::Recovery(int mode)
{
    emit this->RecoverySignal(mode);
}

void MtcManager::SetUmodeToEngin()
{
    emit this->SetUmodeToEnginSignal();
}

void MtcManager::Reset()
{
    emit this->ResetSignal();
}

void MtcManager::SetStreamFormat()
{
    emit this->SetStreamFormatSignal();
}

void MtcManager::GetModelAppVersion()
{
    emit this->GetModelAppVersionSignal();
}
