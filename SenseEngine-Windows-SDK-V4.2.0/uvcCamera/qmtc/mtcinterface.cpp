/************************************************************************
Copyright  : 2018-2028,Shenzhen SenseTime Co.,Ltd
File name  : mtcinterface.cpp
Description: .
Author     : longzeyuan
Date       : 2019-07-23
Version    : V1.0.3
History    : 2019-07-23, Create it by longzeyuan
************************************************************************/
#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include <QtEndian>
#include <QCoreApplication>
#include <QTime>
#include <QThread>
#include <QFile>
#include <QCryptographicHash>
#include <QFileInfo>
#include <qdir.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSettings>
#include <QTextCodec>
#include "ai_camera.h"
#include "mtcinterface.h"
#include "XFFmpeg.h"
#include "XVideoThread.h"

bool MtcInterface::m_isCancel=false;
bool MtcInterface::m_isCancelUpgrade = false;
mapFunc MtcInterface::m_imageData;
QMap<uint64_t, int> MtcInterface::m_imageTotalNum;
QMap<uint64_t, QByteArray> MtcInterface::m_imageTotalData;
MtcInterface* MtcInterface::s_this = nullptr;

MtcInterface::MtcInterface(QObject *parent,void* dev) : QObject(parent)
{
    s_this = this;    
}


MtcInterface::~MtcInterface()
{    
    if(m_parseIDThread != nullptr)
    {
        m_parseIDThread->deleteLater();
        m_parseIDThread = nullptr;
    }

    if(m_thread != nullptr)
    {
        //qDebug() << "MtcInterface::~MtcInterface thread exit | ThreadId is " << QThread::currentThreadId();
        m_thread->quit();
        m_thread->wait(3000);
        m_thread->deleteLater();
        m_thread = nullptr;
    }

    if(dev)
    {
        DeInit(dev);
    }
}

void MtcInterface::initDev()
{    
    if(dev == nullptr)
    {
        int logLever = 0;
        int logTarget = 0;
        bool isExistConfig = false;

        QSettings iniFile("config.ini", QSettings::IniFormat);
        iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));

        QStringList childGroups = iniFile.childGroups();
        if(childGroups.contains("Log"))
        {
            iniFile.beginGroup("Log");
            QStringList  keys = iniFile.childKeys();
            iniFile.endGroup();

            if(keys.contains("LogLever") && keys.contains("LogTarget"))
            {
                isExistConfig = true;
            }
        }

        if(isExistConfig)
        {
            logLever  = iniFile.value("Log/LogLever").toInt();
            logTarget = iniFile.value("Log/LogTarget").toInt();
            SetLogConfig(logLever,logTarget);
        }
        else
        {
            GetLogConfig(&logLever,&logTarget);
            iniFile.setValue("Log/LogLever",logLever);
            iniFile.setValue("Log/LogTarget",logTarget);
            SetLogConfig(logLever,logTarget);
        }
        this->dev = Init();
    }
}

void MtcInterface::setPortName(const QString &portName)
{
    //qDebug() << "-----------MtcInterface::setPortName-----";
    m_portName = portName;
}

void MtcInterface::openSerialPort()
{
    //qDebug() << "-----------MtcInterface::openSerialPort--------------";
    QByteArray devName= m_portName.toLatin1();
    int result = ConnectSerial(dev,devName.data());
    if(result == 0)
    {
        //qDebug() << "Serial open success | Ping test...";
        if(mtc_ping("ping test...") == 0)
        {
            getVersion();
            //qDebug() << "Ping test success";
            bool checkRet = check_model();
            if(!checkRet)
            {
                checkRet = check_model();
                if(!checkRet)
                {
                   checkRet = check_model();
                }
            }

            int mtc_open_ret = mtc_openAutoUploadFaceInfoInFrame();
            if(mtc_open_ret != 0)
            {
                mtc_open_ret = mtc_openAutoUploadFaceInfoInFrame();
                if(mtc_open_ret != 0)
                {
                    mtc_open_ret = mtc_openAutoUploadFaceInfoInFrame();
                }
            }

            emit this->portStatusMsgSignal(true,QString("Connected to %1").arg(m_portName));
        }
        else
        {
            //qDebug() << "Ping test fail";
            result = DisconnectSerial(dev);
            emit this->portStatusMsgSignal(false,devName.append(" Ping test fail"));
            //qDebug() << "[openSerialPort]DisconnectSerial return : " << result;
        }
    }
    else
    {
        //qDebug() << "Serial open fail";
        emit this->portStatusMsgSignal(false,"Serial open fail");
    }
}

void MtcInterface::closeSerialPort()
{
    DisconnectSerial(dev);
}

int MtcInterface::mtc_openAutoUploadFaceInfoInFrame()
{
    int openAutoret = OpenAutoUploadFaceInfoInFrame(dev);
    //qDebug() << "openAutoret = openAutoUploadFaceInfoInFrame(dev);" << openAutoret;
    if(openAutoret == 0)
    {
        XFFmpeg::Get()->setIsFrameReport(true);
    }

    return openAutoret;
}

int MtcInterface::mtc_openAutoUploadFaceInfoInFrameResult()
{
    int openAutoret = OpenAutoUploadFaceInfoInFrame(dev);
    //qDebug() << "openAutoret = openAutoUploadFaceInfoInFrame(dev);" << openAutoret;
    if(openAutoret == 0)
    {
        XFFmpeg::Get()->setIsFrameReport(true);
    }

    bool flag = openAutoret ==0 ? true : false;
    emit this->openFrameAutoResult(flag);

    return openAutoret;
}

int MtcInterface::mtc_closeAutoUploadFaceInfoInFrameResult()
{
    int closeAutoret = CloseAutoUploadFaceInfoInFrame(dev);
    //qDebug() << "openAutoret = closeAutoUploadFaceInfoInFrame(dev);" << closeAutoret;
    if(closeAutoret == 0)
    {
        XFFmpeg::Get()->setIsFrameReport(false);
    }

    bool flag = closeAutoret ==0 ? true : false;
    emit this->closeFrameAutoResult(flag);

    return closeAutoret;
}

int MtcInterface::mtc_antiFlickerSignalResult(int isEnable,int RGBIR,int HZ)
{
    int FlickerHzret = SetDeviceNoFlickerHz(dev, RGBIR, HZ, isEnable);
    //qDebug() << "FlickerHzret = setDeviceNoFlickerHz:" << FlickerHzret;

    bool flag = FlickerHzret ==0 ? true : false;
    emit this->setDeviceNoFlickerHzSignal(flag);

    return FlickerHzret;
}

int MtcInterface::mtc_getAntiFlickerSignalResult(int RGBIR)
{
    uchar pHz;
    uchar isEnable;
    int ret = GetDeviceNoFlickerHz(dev, static_cast<char>(RGBIR) ,&pHz, &isEnable);

    emit this->getDeviceNoFlickerHzSignal(ret, RGBIR, pHz, isEnable);

    return ret;
}

int MtcInterface::mtc_ping(QString data)
{
    //qDebug() << "MtcInterface::mtc_ping thread is " << QThread::currentThread();
    //qDebug() << "[MtcInterface::mtc_ping] ";
    m_pingData = data;
    //qDebug()<<"data:"<<data;
    //qDebug()<<"m_pingData.length():"<<m_pingData.length();

    QByteArray qbam_pingData = m_pingData.toLocal8Bit();
    int result = Ping(dev,qbam_pingData.data() ,qbam_pingData.length());
    if(result != 0)
    {
        result = Ping(dev,qbam_pingData.data() ,qbam_pingData.length());
        if(result != 0)
        {
            result = Ping(dev,qbam_pingData.data() ,qbam_pingData.length());
        }
    }
    bool flag = result ==0 ? true : false;
    emit this->pingResult(flag);
    return result;
}

bool MtcInterface::check_model()
{
    int FrameWidth = -1;
    int FrameHeight = -1;
    //qDebug() << "[MtcInterface::check_model]";
    bool ret = false;
    DeviceType devType = DEVICE_UNKNOW;
    char buffer[128];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0, buf_size);
    GetDevModel(dev, buffer, buf_size);
    QString model(buffer);
    //qDebug()<<"check_model:  " << model;
    if(model.contains("M20"))
    {
        ret = true;
        devType = DEVICE_M20;
    }
    else if (model.contains("SENM2HD02") || model.contains("SENM2HD12") || model.contains("M20S"))
    {        
        //M20 S
        ret = true;
        devType = DEVICE_M20S;
        RegisterQRCodeReportCb(dev, MtcInterface::getQRCodeReportHandle);
    }
    else if(model.contains("M10"))
    {
        uchar ScreenChar = 0;
        int retScreen = GetScreenDirection(dev,&ScreenChar);
        if(retScreen == 0)
        {
            //qDebug()<<"getScreenDirection:"<<(int)ScreenChar;
            //0x01：Landscape mode0°
            //0x02: Vertical screen mode90°
            //0x03：Landscape mode180°
            //0x04：Vertical screen mode270°
            GetResolution(dev, &FrameWidth, &FrameHeight);
            if(ScreenChar == 1 || ScreenChar == 3)
            {
                //width*height 1280*720 640*360
                if(FrameWidth == 1280 && FrameHeight== 720)
                {

                }
                else if(FrameWidth == 640 && FrameHeight== 360)
                {

                }
                else if(FrameWidth == 640 || FrameHeight == 640 || FrameWidth == 360 || FrameHeight == 360)
                {
                    FrameWidth  = 640;
                    FrameHeight = 360;
                    SetResolution(dev,0x01);
                }
                else
                {
                    FrameWidth  = 1280;
                    FrameHeight = 720;
                    SetResolution(dev,0x00);
                }
            }
            else if(ScreenChar == 2 || ScreenChar == 4)
            {
                //width*height 720*1280 360*640
                if(FrameWidth == 720 && FrameHeight== 1280)
                {

                }
                else if(FrameWidth == 360 && FrameHeight== 640)
                {

                }
                else if(FrameWidth == 360 || FrameHeight == 360 || FrameWidth == 640 || FrameHeight == 640)
                {
                    FrameWidth = 360;
                    FrameHeight = 640;
                    SetResolution(dev,0x01);
                }
                else
                {
                    FrameWidth = 720;
                    FrameHeight = 1280;
                    SetResolution(dev,0x00);
                }
            }
        }
        int reoResult = -1;
        int reoMode = REO_AI_CONFIG;
        char reoBuffer[2048];
        uint reo_buf_size = sizeof(reoBuffer);
        memset(reoBuffer, 0, reo_buf_size);
        //qDebug() << "get AIconfig";
        reoResult = GetRecConfig(dev, reoBuffer, reo_buf_size, reoMode);
        //qDebug() << "reoResult is:" << reoResult;
        if(reoResult == 0)//After the restart, the buffer is empty after the first success
        {
            if(reoBuffer[0] == '\0')
            {
                //ret = -1;//The default recognize_mode: 1
                ret = false;
            }
            else
            {
                QString Qaiconfig = reoBuffer;
                if(Qaiconfig.contains("\"recognize_mode\": 1",Qt::CaseInsensitive))
                {
                    ret = true;
                    devType = DEVICE_M10;
                }
            }
        }
    }
    emit this->checkModelM20(devType,FrameWidth,FrameHeight);
    return ret;
}

void MtcInterface::mtc_query_face(QString imageId)
{
    int result = -1;

    if(!imageId.isEmpty())
    {
        QByteArray imageArray = imageId.toUtf8();
        result = QueryFace(dev,imageArray.data(),imageArray.size());
        //qDebug() << "delete result:" << result;
        emit queryFaceResult(result, imageId);
    }
}

void MtcInterface::_deleteImage(int mode, QString imageId)
{
    int result = -1;

    if(!imageId.isEmpty())
    {
        QByteArray imageArray = imageId.toUtf8();
        imageArray.truncate(MTC_IMAGE_ID_LENGTH);
        result = DeleteFace(dev,mode,imageArray.data(),imageArray.length());
        //qDebug() << "delete result:" << result;
    }
    else
    {
        result = DeleteFace(dev,mode);
    }

    emit this->deleteImageResult(mode,result);
}

void MtcInterface::_addImage(int mode, QString imageId, QString filePath)
{
    int result = -1;
    QFileInfo Imageinfo(filePath);
    if(Imageinfo.size() > 100 * 1024)
    {
        result = this->tryAddImageSlice(imageId,filePath,Imageinfo.size());
    }
    else
    {
        result = this->tryAddImage(mode,imageId,filePath);
    }

    emit this->addImageResult(result);
}

int MtcInterface::tryAddImage(int mode, QString imageId, QString filePath)
{
    int result = -1;
    result = mtcAddImage(mode,imageId,filePath);
    if(result == -1)
    {
        result = mtcAddImage(mode,imageId,filePath);
    }
    if(result == -1)
    {
        result = mtcAddImage(mode,imageId,filePath);
    }
    return result;
}

int MtcInterface::tryAddImageSlice(QString imageId,QString filePath,uint fileLen)
{
    QByteArray imageIdArray;
    int result = -1;
    if(!imageId.isEmpty())
    {
        imageIdArray = imageId.toUtf8();
    }
    else
    {
        return 16;
    }

    char sliceData[MTC_UPGRADE_SEGMENT_LENGTH] = {0};
    uint sliceLen = 0;
    int position = 0;
    int islastSlice = 0;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }

    while (!file.atEnd())
    {
        sliceLen = file.read(sliceData, MTC_UPGRADE_SEGMENT_LENGTH);
        if(sliceLen <= 0)
        {
            break;
        }

        if(file.atEnd())
        {
            islastSlice = 1;
        }

        result = AddFaceSlice(dev, imageIdArray.data(), imageIdArray.length(), fileLen, position, islastSlice, sliceData, sliceLen);
        if(result != 0)
        {
            break;
        }

        position += sliceLen;
    }
    file.close();
    return result;
}

int MtcInterface::inputLibrary(int mode, QString filePath)
{
    int result = -1;
    QFileInfo Imageinfo(filePath);
    if(Imageinfo.size() >= 4* 1024 * 1024)
    {
        emit this->inputLibraryResult(result, "The picture is too large for more than 4M");
    }
    else
    {
        if(Imageinfo.size() > 300 * 1024)
        {
            result = this->imputLibrarySlice(mode,filePath);
        }
        else
        {
            result = this->imputLibrarySingle(mode,filePath);
        }
    }

    return result;
}

//Not divided
int MtcInterface::imputLibrarySingle(int mode, QString filePath)
{
    int result = -1;
    void *retBuffer = nullptr;
    unsigned int retBuf_size = 0;
    mtc_image image;
    QByteArray imageData;

    if(mode == MTC_ADD_IMAGE_LOCAL_CAPTURE)
    {
        result = StaticFaceCompare(dev, &retBuffer , &retBuf_size);
    }
    else if(mode == MTC_ADD_IMAGE_REMOTE_INPUT)
    {
        if(!filePath.isEmpty())
        {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly))
            {
                return result;
            }
            imageData = file.readAll();
            image.image = (unsigned char*)imageData.data();
            image.image_length = file.size();
            //qDebug() << "mtcAddImage length is " << image.image_length;
            file.close();
        }

        result = StaticFaceCompareReturnImage(dev,image.image, image.image_length, &retBuffer , &retBuf_size);
    }

    emit this->inputLibraryResult(result, static_cast<char*>(retBuffer));
    if(retBuffer)
    {
        free(retBuffer);
        retBuffer = nullptr;
    }

    return result;
}

//divided
int MtcInterface::imputLibrarySlice(int mode, QString filePath)
{
    int result = -1;
    char sliceData[MTC_UPGRADE_SEGMENT_LENGTH] = {0};
    uint sliceLen = 0;
    int position = 0;
    int islastSlice = 0;
    void *retBuffer = nullptr;
    unsigned int retBuf_size = 0;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    while (!file.atEnd())
    {
        sliceLen = file.read(sliceData, MTC_UPGRADE_SEGMENT_LENGTH);
        if(sliceLen <= 0)
        {
            break;
        }

        if(file.atEnd())
        {
            islastSlice = 1;
        }
        result = StaticFaceCompareSlice(dev, mode, file.size(), position, islastSlice, sliceData, sliceLen, &retBuffer, &retBuf_size);
        if(result != 0)
        {
            break;
        }
        position += sliceLen;
    }
    file.close();
    emit this->inputLibraryResult(result, static_cast<char*>(retBuffer));

    if(retBuffer)
    {
        free(retBuffer);
        retBuffer = nullptr;
    }
    return result;
}

int MtcInterface::mtcAddImage(int mode, QString imageId, QString filePath)
{
    mtc_image image;
    QByteArray imageIdArray;
    QByteArray imageData;
    int result = -1;
    if(!imageId.isEmpty())
    {
        imageIdArray = imageId.toUtf8();
    }
    if(!filePath.isEmpty())
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return result;
        }
        imageData = file.readAll();
        file.close();
    }
    switch (mode) {
    case MTC_ADD_IMAGE_LOCAL_CAPTURE:
    {
        //qDebug() << "MTC_ADD_IMAGE_LOCAL_CAPTURE";
        result = AddFace(dev, imageIdArray.data(), imageIdArray.length());
        //qDebug() << "result is:" << result;
        break;
    }
    case MTC_ADD_IMAGE_REMOTE_INPUT:
    {
        //qDebug() << "MTC_ADD_IMAGE_REMOTE_INPUT" << "data length is:" << imageData.length();
        result = AddFaceByImage(dev, imageIdArray.data(), imageIdArray.length(), imageData.data(), imageData.length());
        //qDebug() << "result is:" << result;
        break;
    }
    case MTC_ADD_IMAGE_CAPTURE_RETURN:
    {
        //qDebug() << "MTC_ADD_IMAGE_CAPTURE_RETURN" << "data length is:" << imageData.length();
        void *retData = nullptr;
        unsigned int length = 0;
        result = AddFaceReturnImage(dev, imageIdArray.data(), imageIdArray.length(), &retData, &length);
        //qDebug() << "result is:" << result << " length " << length;
        if(result == 0x00)
        {
            QDir dir;
            if(!dir.exists("BaseImage")){
                dir.mkdir("BaseImage");
            }
            QFile file(QString("BaseImage/%1%2").arg(imageIdArray.data()).arg(".jpg"));
            file.open(QIODevice::WriteOnly);
            QByteArray tmp(static_cast<char*>(retData) , length);
            file.write(tmp);
            file.flush();
            file.close();
            free(retData);
        }
        break;
    }
    }
    return result;
}

int MtcInterface::mtcAddFeature(QString featureId, QString filePath)
{
    QByteArray featureData;
    QByteArray featureArray;
    int result = -1;
    if(!featureId.isEmpty())
    {
        featureArray = featureId.toUtf8();
        featureArray.truncate(MTC_IMAGE_ID_LENGTH);
        //qDebug() << "mtcAddImage featureid is " << featureId << "   length " << featureId.length() << "   featureA length " << featureArray.length();
    }
    if(!filePath.isEmpty())
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadWrite))
        {
            return result;
        }
        featureData = file.readAll();
        file.close();
    }

    result = AddFeature(dev, featureArray.data(), featureArray.length(), featureData.data(), featureData.length());
    emit this->addImageResult(result);
    return result;
}

void MtcInterface::addImageBatch(QString filePath)
{
    QString fileName = filePath.split("/").last().section(".",0,-2);
    QFileInfo Imageinfo(filePath);
    if(Imageinfo.size() < (4*1024*1024))
    {
        int result = -1;
        if(fileName.contains('\\') || fileName.contains('/') || fileName.contains(':') ||
           fileName.contains('*')  || fileName.contains('?') || fileName.contains('<') ||
           fileName.contains('>')  || fileName.contains('|') )
        {
            this->addImageBatchStatus(-11,filePath);
            return;
        }

        if(Imageinfo.size() >102400)
        {
            result = this->tryAddImageSlice(fileName,filePath,Imageinfo.size());
        }
        else
        {
            result = this->tryAddImage(MTC_ADD_IMAGE_REMOTE_INPUT,fileName,filePath);
        }

        this->addImageBatchStatus(result,filePath);
    }
    else
    {
        this->addImageBatchStatus(-10,filePath);
    }
}

void MtcInterface::openCamera()
{
    QString cameraName(XFFmpeg::Get()->getDeviceName());
    XFFmpeg::Get()->Open(cameraName);
    XVideoThread::Get()->start();
    ResumeRecognize(dev);
}

void MtcInterface::closeCamera()
{
    XVideoThread::Get()->Close();
    XFFmpeg::Get()->Close();
    PauseRecognize(dev);
}

void MtcInterface::cancelAddImageBatch()
{
    m_isCancel = true;
}

void MtcInterface::cancelUpgradeBatch()
{

    m_isCancelUpgrade = true;
}

void MtcInterface::upgradeSlots(QString filePath)
{
    if(filePath.isEmpty()){
        return;
    }

    if(upgradeUpload(filePath) != 0)
    {
        emit this->uploadStatus(true,tr("Upgrade file upload failed !"),false);
    }else
    {
        emit this->uploadStatus(false,tr("Upgrade file upload complete !"),false);

        if(upgradeApply(filePath) != 0)
        {
            emit this->uploadStatus(true,tr("upgrade apply failed.\n"),false);
        }
        else
        {
            QString msg =  tr("<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0//EN' 'http://www.w3.org/TR/REC-html40/strict.dtd'>"
            "<html><head><meta name='qrichtext' content='1' /><style type='text/css'>"
            "p, li { white-space: pre-wrap; }"
            "</style></head><body style=' font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;'>"
            "<p style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'><span style=' color:#000000;'>upgrade apply ok.</span></p>"
            "<p style='-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#d69545;'><br /></p>"
            "<p style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'><span style=' font-size:12pt; color:#ef2929;'>Warning:</span></p>"
            "<p style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'><span style=' font-size:12pt; color:#ef2929;'>The hardware starts rebooting, please do not turn off the power!</span></p>"
            "<p style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;'><span style=' font-size:12pt; color:#ef2929;'>Please wait about 40 seconds,then choice the camera and COM again.</span></p></body></html>");
            emit this->uploadStatus(true,msg,true);            
        }
    }
}

int MtcInterface::applyUpgradePackage(char *md5, int length)
{
    int result = -1;
    result = ApplyUpgrade(dev,md5,length);
    //qDebug() << "applyUpgradePackage result:" << result;
    return result;
}

int MtcInterface::mtcStartOnetoNumRecognize(int reomode, int mulmode)
{
    int result = -1;
    result = StartOnetoNumRecognize(dev, reomode, mulmode);

    return result;
}

int MtcInterface::mtc_start_onetoone_recognize(int reomode, int mulmode, QString filePath)
{
    int result = -1;
    QFileInfo Imageinfo(filePath);
    int image_size = Imageinfo.size();
    if(image_size >= 4 * 1024 * 1024)
    {
        emit this->onetooneRecognizeResult(result);
    }
    else
    {
//        result = this->mtcStartO2ORecognizeSlice(reomode,mulmode,filePath);
        if(image_size > 300 * 1024)
        {
            result = this->mtcStartO2ORecognizeSlice(reomode,mulmode,filePath);
        }
        else
        {
            result = this->mtcStartO2ORecognizeSingle(reomode,mulmode,filePath);
        }
    }
    return result;
}

//Not divided
int MtcInterface::mtcStartO2ORecognizeSingle(int reomode, int mulmode, QString filePath)
{
    int result = -1;
    QByteArray imageData;
    if(!filePath.isEmpty())
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return result;
        }
        imageData = file.readAll();
        file.close();
    }
    result = StartOnetoOneRecognize(dev, reomode, mulmode, imageData.data(), imageData.length());
    emit this->onetooneRecognizeResult(result);
    return result;
}

//Divided
int MtcInterface::mtcStartO2ORecognizeSlice(int reomode, int mulmode, QString filePath)
{
    int result = -1;
    char sliceData[MTC_UPGRADE_SEGMENT_LENGTH] = {0};
    uint sliceLen = 0;
    int position = 0;
    int islastSlice = 0;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    while (!file.atEnd())
    {
        sliceLen = file.read(sliceData, MTC_UPGRADE_SEGMENT_LENGTH);
        if(sliceLen <= 0)
        {
            break;
        }

        if(file.atEnd())
        {
            islastSlice = 1;
        }
        result = StartOnetoOneRecognizeSlice(dev, reomode, mulmode, file.size(), position, islastSlice, sliceData, sliceLen);
        if(result != 0)
        {
            break;
        }
        position += sliceLen;
    }
    file.close();
    emit this->onetooneRecognizeResult(result);
    return result;
}

int MtcInterface::mtc_query_feature(QString imageId)
{
    int result = -1;
    char buffer[2048];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0x00, buf_size);
    QByteArray featureIdArray;
    featureIdArray = imageId.toUtf8();
    result = QueryFeature(dev, featureIdArray, featureIdArray.length(), buffer, buf_size);
    //qDebug() << "result:" << result;
    if(0 == result)
    {
        QString tempFeadata(buffer);
        //        QFile file("feature");
        QFile file(QString("%1.feature").arg(imageId));
        file.open(QIODevice::ReadWrite);
        file.write(tempFeadata.toLatin1());
        file.close();
    }
    emit this->queryFeatureResult(result);
    return result;
}

int MtcInterface::mtc_set_frame(int frame)
{
    int result = -1;    
    result = SetFrameRate(dev, frame);
    emit this->setFrameResult(result);
    return result;
}

int MtcInterface::mtc_get_frame()
{
    int result = -1;
    uint frameRate = 0;
    result = GetFrameRate(dev, &frameRate);
    emit this->getFrameResult(result, static_cast<int>(frameRate));
    return result;
}

int MtcInterface::mtc_set_irlight(int irlight, int closeTime)
{
    int result = -1;
    result = SetIRlight(dev, irlight, closeTime);
    emit this->setIrLightResult(result);
    return result;
}

int MtcInterface::mtc_get_irlight()
{
    int result = -1;
    uchar luminamce = 0;
    uchar closeTime = 0;
    result = GetIRlight(dev, &luminamce, &closeTime);

    //qDebug() << "ir light value is "<<luminamce << "    closeTime: " << closeTime;
    emit this->getIrLightResult(result, luminamce, closeTime);
    return result;
}

int MtcInterface::mtc_set_logconfig(int logLever ,int logTarget)
{
    int result = -1;
    int lever = 0;
    int target = 0;
    GetLogConfig(&lever,&target);
    result = SetLogConfig(logLever,logTarget);
    if(result == 0)
    {
        QSettings iniFile("config.ini", QSettings::IniFormat);
        iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));

        iniFile.setValue("Log/LogLever",logLever);
        iniFile.setValue("Log/LogTarget",logTarget);
    }

    emit this->setLogConfigResult(result);
    return result;
}

int MtcInterface::mtc_get_logconfig()
{
    int result = -1;
    int logLever = 0;
    int logTarget = 0;
    bool isExistConfig = false;

    QSettings iniFile("config.ini", QSettings::IniFormat);
    iniFile.setIniCodec(QTextCodec::codecForName("utf-8"));

    QStringList childGroups = iniFile.childGroups();
    if(childGroups.contains("Log"))
    {
        iniFile.beginGroup("Log");
        QStringList  keys = iniFile.childKeys();
        iniFile.endGroup();

        if(keys.contains("LogLever") && keys.contains("LogTarget"))
        {
            isExistConfig = true;
        }
    }

    if(isExistConfig)
    {
        logLever  = iniFile.value("Log/LogLever").toInt();
        logTarget = iniFile.value("Log/LogTarget").toInt();
        result = 0;
    }
    else
    {
        result = GetLogConfig(&logLever, &logTarget);
        iniFile.setValue("Log/LogLever",logLever);
        iniFile.setValue("Log/LogTarget",logTarget);
    }

    emit this->getLogConfigResult(result, logLever, logTarget);
    return result;
}

int MtcInterface::mtc_set_RecognizeCount(int recognizeCount, int livingCount)
{
    int result = -1;
    result = SetRecognizeCount(dev, recognizeCount,livingCount);
    emit this->setRecognizeCountResult(result);
    return result;
}

int MtcInterface::mtc_get_RecognizeCount()
{
    int result = -1;
    char buffer[2] = {0};
    uchar recognizeCount;
    uchar livingCount;

    result = GetRecognizeCount(dev, &recognizeCount, &livingCount);



    emit this->getRecognizeCountResult(result, recognizeCount,livingCount);
    return result;
}

int MtcInterface::mtc_set_QRCodeSwitch(int mode, int interval)
{
    int result = -1;
    result = SetQRCodeSwitch(dev, mode, interval);
    emit this->setQRCodeSwitchResult(mode,interval,result);
    return result;
}

int MtcInterface::mtc_get_cpuTemperature()
{
    int result = -1;
    uint temp = 0;
    result = GetCpuTemperature(dev, &temp);    
    emit this->getCpuTemperatureResult(result,temp);
    return result;
}

int MtcInterface::mtc_get_aesensormodel()
{
    int result = -1;
    uint sensorModel = 0;
    result = GetAESensorModel(dev, &sensorModel);
    emit this->getAESensorModelResult(result, sensorModel);
    return result;
}

int MtcInterface::mtc_open_TemlateUpdate()
{
//    0x00: Close the update
//    0x01: Open the update
    int result = -1;
    result = SetTemplateUpdate(dev, 1);
    emit this->setTemlateUpdateResult(result,1);
    return result;
}
int MtcInterface::mtc_close_TemlateUpdate()
{
    int result = -1;
    result = SetTemplateUpdate(dev, 0);
    emit this->setTemlateUpdateResult(result,0);
    return result;
}
int MtcInterface::mtc_get_TemlateUpdate()
{
    int result = -1;
    uchar buffer = 0;

    result = GetTemplateUpdate(dev, &buffer);

    int TemlateUpdateMode = (unsigned char)buffer;

    emit this->getTemlateUpdateResult(result, TemlateUpdateMode);
    return result;
}

int MtcInterface::mtc_set_aiconfig(QString aiconfig)
{
    int result = -1;
    int mode = REO_AI_CONFIG;
    QByteArray reoconfig;
    reoconfig = aiconfig.toUtf8();
    //qDebug() << "aiconfig is:" << aiconfig;

    QJsonObject json;
    json = QJsonDocument::fromJson(reoconfig).object();
    if(!json.contains("enable_mask_sign"))
    {
        json.insert("enable_mask_sign",false);
    }
    if(!json.contains("enable_safety_hat"))
    {
        json.insert("enable_safety_hat",false);
    }

    QJsonDocument doc(json);
    reoconfig =  doc.toJson();
    //qDebug() << "aiconfig is:" << reoconfig;
    //result = setReoconfig(dev, (char*)aiconfig.data(), aiconfig.length());
    result = SetRecConfig(dev, reoconfig.data(), reoconfig.length(), mode);
    //qDebug() << "result is:" << result << "length :" << reoconfig.length();
    emit this->setAiConfigResult(result);
    return result;
}

int MtcInterface::mtc_get_aiconfig()
{
    int result = -1;
    int mode = REO_AI_CONFIG;
    char buffer[2048];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0, buf_size);
    //qDebug() << "get AIconfig";

    result = GetRecConfig(dev, buffer, buf_size, mode);
    //qDebug() << "result is:" << result;
//    if(result == 0)//After the restart, the buffer is empty after the first success
//    {
//        qDebug() << "result == 0" ;
//        if(buffer[0] == '\0')
//        {
//            const char *aiconfig = "{\n\"work_mode\": 0,\n\"recognize_mode\": 1,\n\"face_mode\": 1,\n\"enable_qr_code\": false,\n\"enable_track_box\": true,\n\"liveness_thresh\": 0.98,\n\"recognition_thresh\": 0.90,\n\"verify_thresh\": 0.90,\n\"recognition_interval\": 5,\n\"report_interval\": 5,\n\"max_face_cnt\": 4,\n\"face_size\": 30,\n\"enable_feature_export\": true,\n\"enable_image_export\": true,\n\"enable_mask_sign\": false,\n\"enable_safety_hat\": false,\n\"ae_mode\": 1,\n\"ae_flicker_freq\":0\n }";
//            strcpy(buffer,aiconfig);
//        }
//        qDebug() << "result:"<<buffer ;
//    }
    emit this->getAiconfigResult(buffer);
    return result;
}

int MtcInterface::mtc_get_lightsensor()
{
    int result = -1;    
    uint lightsensor = 0;
    result = GetLuminousSensitivityThreshold(dev, &lightsensor);
    emit this->getLightSensorResult(result, lightsensor);
    return result;
}

int MtcInterface::mtc_set_videosize(int size)
{
    int result = SetResolution(dev, size);
    emit this->setVideoSizeResult(result);
    return result;
}

int MtcInterface::mtc_get_videosize()
{
    int result = -1;
    int width  = 0;
    int height = 0;
    result = GetResolution(dev, &width, &height);
    emit this->getVideoSizeResult(result, width, height);
    return result;
}

int MtcInterface::mtc_set_screenmode(int mode)
{
    int result = -1;
    result = SetScreenDirection(dev, mode);
    //qDebug() << "set Screen Mode result:"<< result << "mode:" << mode ;
    emit this->setScreenModeResult(result);
    if(result == 0)
    {        
        int result2 = Reboot(dev, 0);
        emit this->rebootResult(result);        
    }
    return result;
}

int MtcInterface::mtc_set_rotation(int angle)
{
    int result = -1;
    result = SetRotateAngle(dev, angle);
    emit this->setRotateAngleResult(result);
    return result;
}

int MtcInterface::mtc_query_recognize(int mode)
{
    int result = -1;
    return result;
}

int MtcInterface::mtc_reboot(int mode)
{
    int result = -1;
    result = Reboot(dev, mode);
    emit this->rebootResult(result);    
    return result;
}

int MtcInterface::mtc_recovery(int mode)
{
    int result = -1;
    result = Recovery(dev, mode);
    emit this->recoveryResult(result);    
    return result;
}

int MtcInterface::mtc_setUmodeToEngin()
{
    int result = -1;
    result = SetUmodeToEngin(dev);
    emit this->setUmodeToEnginResult(result);
    return result;
}

int MtcInterface::mtc_reset()
{
    int result = -1;
    result = Reset(dev);
    emit this->resetResult(result);
    return result;
}

int MtcInterface::mtc_setStreamFormat()
{
    int result = -1;
    result = SetStreamFormat(dev, 0);
    emit this->setStreamFormatResult(result);
    return result;
}

int MtcInterface::mtc_getModelAppVersion()
{
    int result = -1;
    char buffer[1024];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0, buf_size);
    result = GetDevModelAppVer(dev, buffer, buf_size);
    emit this->getModelAppVersionResult(result, buffer);
    return result;
}

/**
 * @brief MtcInterface::mtc_opencamera
 * @param mode 0x00：RGBcamera 0x01：IRcamera
 * @return
 */
int MtcInterface::mtc_opencamera(int mode)
{
    int result = -1;
    uchar modeCameraStream = 0;
    int modeRet = GetCameraStream(dev,mode,&modeCameraStream);
    {
        if(0 == modeRet)
        {
            if(0x01 == modeCameraStream)
            {
                //opend
                result = 1;
                emit this->opencameraResult(result);
                return result;
            }
        }
    }

    result = SetCameraStream(dev, OPEN_CAMERA, mode);
    if(0 == result)
    {
        if(RGB_CAMERA == mode)
        {
            //RGB
            int switchRet = SwitchCamRgbIr(dev,mode);
            if(0 == switchRet)
            {
                XFFmpeg::Get()->setFaceRectEnable(true);

                QString cameraName(XFFmpeg::Get()->getDeviceName());
                XFFmpeg::Get()->Close();
                XFFmpeg::Get()->Open(cameraName);
            }
        }
        else
        {
            //IR
            int switchRet = SwitchCamRgbIr(dev,mode);
            if(0 == switchRet)
            {
                XFFmpeg::Get()->setFaceRectEnable(false);

                QString cameraName(XFFmpeg::Get()->getDeviceName());
                XFFmpeg::Get()->Close();
                XFFmpeg::Get()->Open(cameraName);
            }
        }
    }
    emit this->opencameraResult(result);
    return result;
}

int MtcInterface::mtc_closecamera(int mode)
{
    int result = -1;
//    pauseRecognize(dev);
    //qDebug() << "pauseRecognize ";
    result = SetCameraStream(dev, CLOSE_CAMERA, mode);
    //qDebug() << "mtc_closecamera result:"<< result << "mode:" << mode ;
    emit this->closecameraResult(result);
    return result;
}

int MtcInterface::mtc_getidlist()
{
    int result = -1;
    char buffer[685*1024];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0x00, buf_size);

    result = GetDeviceFaceID(dev, buffer, buf_size);
//    if(result != 0)
//    {
//        result = getDeviceFaceID(dev,buffer,buf_size);
//    }
    //qDebug() << "result:" << result;
    if(buf_size != 0)
    {
        QString tempFeadata(buffer);

        //        QFile file("D:/testfeature/IDList");
        QFile file("IDList");
        //        file.open(QIODevice::ReadWrite);//Will write in substitution, if not long enough will keep the last result.
        file.open(QIODevice::WriteOnly);
        //        file.write(tempFeadata.toLatin1());//Chinese/Japanese characters not supported
        file.write(tempFeadata.toUtf8());
        file.close();
    }

    emit this->getIDListResult(result);
    return result;
}

int MtcInterface::mtc_openAutoReport(int imageMode)
{
    QDir dir;
    if(!dir.exists("image")){
        dir.mkdir("image");
    }

    if(m_parseIDThread == nullptr)
    {
        m_parseIDThread = new parseImageDataThread();
    }
    if(m_thread == nullptr)
    {
        m_thread = new QThread();
    }else
    {
        m_thread->quit();
    }

    int result = -1;
    result = OpenAutoUploadAiInfo(dev,0x00,imageMode);
    RegisterRecoReportCb(dev, getReoReportDataHandle);
    RegisterTrackReportCb(dev, getReoTrackHandle);
    RegisterImageReportCb(dev, getReoImageHandle);
    RegisterFeatureReportCb(dev, getReoFeatureHandle);
    emit this->openAutoReportResult(result);
    m_parseIDThread->moveToThread(m_thread);
    connect(this,&MtcInterface::DoParseImageData,m_parseIDThread,&parseImageDataThread::doParseImageData);
    //qDebug() << "MtcInterface::mtc_openAutoReport thread start | ThreadId is " << QThread::currentThreadId();
    m_thread->start();

    return result;
}

//void MtcInterface::getCameraStatusHandle(char connectStatus, char* camName, char camNameLength)
//{
//    qDebug()  << "[MtcInterface::getCameraStatusHandle]"
//              << "connectStatus : " << connectStatus
//              << "camName : "       << camName
//              << "camNameLength : " << camNameLength;;
//    QString cameraName(XFFmpeg::Get()->getDeviceName());
//    XFFmpeg::Get()->Close();
//    XFFmpeg::Get()->Open(cameraName);
//}

//void MtcInterface::getSerialStatusHandle(char connectStatus, char* serialName, char serialNameLength)
//{
//    QByteArray temp(serialName,serialNameLength);
//    qDebug()  << "[MtcInterface::getSerialStatusHandle] "
//              << "connectStatus : "    << static_cast<int>(connectStatus)
//              << "serialNameLength : " << static_cast<int>(serialNameLength)
//              << "serialName :"        << QString(temp);
//}

void MtcInterface::getReoReportDataHandle(char * value, int len) {
    if(len > 4)
    {
        QByteArray tmp(value, len);
        uint length  = tmp.at(0) & 0x000000FF;
        length |= ((tmp.at(1) <<8)&0x0000FF00);
        length |= ((tmp.at(2) <<16)&0x00FF0000);
        length |= ((tmp.at(3) <<24)&0xFF000000);
        //qDebug() << "[getReoReportDataHandle]lengths: "<<length << ", len: " << len;

//        int length =value[0] | value[1] << 8 | value[2] << 16 | value[3] << 24;
//      qDebug() << "MtcInterface::getReoReportDataHandle length: "<<length << "len: " << len;
        if(length == len - 4)
        {
            QByteArray tmp(value, len);
            QByteArray TrackJson = tmp.mid(4, length);            
            TrackJson = "{\"track_info\":["+TrackJson + "]}";
            //qDebug() << "[getReoReportHandle]parseVerifyData:"<< TrackJson.data();
            XFFmpeg::Get()->setJsonData(TrackJson);
        }else
        {
            //qDebug() << "[getReoReportDataHandle] size not suit ";
        }
    }
}

void MtcInterface::getReoTrackHandle(char * value, int len) {
    if(len > 4)
    {
        QByteArray tmp(value, len);
        uint length  = tmp.at(0) & 0x000000FF;
        length |= ((tmp.at(1) <<8)&0x0000FF00);
        length |= ((tmp.at(2) <<16)&0x00FF0000);
        length |= ((tmp.at(3)<<24)&0xFF000000);
        //qDebug() << "[getReoTrackHandle]length: "<<length << ", len: " << len;
        if(length == len - 4)
        {
            QByteArray TrackJson = tmp.mid(4, length);
            //qDebug() << "[getTrackReportHandle]parseVerifyData:"<<TrackJson.data();
            XFFmpeg::Get()->setJsonData(TrackJson);
        }else
        {
            //qDebug() << "[getReoTrackHandle] value size no suit.";
        }
    }
}

void MtcInterface::getReoImageHandle(char * value, int len) {
    if (len > 4)
    {
        //QByteArray tmp = QByteArray::fromRawData(value, len);
        QByteArray tmp(value, len);
        //qDebug() << "[getReoImageHandle]: " << tmp << ", len: " << len;
        uint64_t stamps  = tmp.at(0) & 0x000000FF;
        stamps |= ((tmp.at(1) <<8)&0x0000FF00);
        stamps |= ((tmp.at(2) <<16)&0x00FF0000);
        stamps  |= ((tmp.at(3)<<24)&0xFF000000);
        int sliceTotal = tmp.at(4)&0x000000FF;
        sliceTotal |= ((tmp.at(5) <<8)&0x0000FF00);
        int sliceId = tmp.at(6) & 0x000000FF;
        sliceId |= ((tmp.at(7) <<8)&0x0000FF00);
        //qDebug() << "Test Debug 858  [getReoImageHandle]timestamps : " << stamps << "total: " << sliceTotal << "sliceId: " << sliceId; //Test Debug 858
        if (!m_imageData.contains(stamps))
        {
            QMap<int, QByteArray> tmpData;
            tmpData.insert(sliceId, tmp.mid(8));
            m_imageData.insert(stamps, tmpData);
            m_imageTotalNum.insert(stamps, sliceTotal);

            if(sliceTotal == 1)
            {
                emit s_this->DoParseImageData(stamps, tmpData.value(sliceId));
                m_imageTotalData.clear();
                m_imageData.clear();
                m_imageTotalNum.clear();
            }
        }
        else
        {
            QMap<int, QByteArray>  tmpData = m_imageData[stamps];
            tmpData.insert(sliceId, tmp.mid(8));
            m_imageData[stamps] = tmpData;
            if (m_imageData.size() >1) {
                QMapIterator<uint64_t, QMap<int, QByteArray>> i(m_imageData); // Delete previously incomplete data
                while(i.hasNext())
                {
                    i.next();
                    if (i.key() != m_imageData.lastKey())
                    {
                        m_imageData.remove(i.key());
                        m_imageTotalNum.remove(i.key());
                    }
                }
            }
            if (m_imageData.size() == 1)
            {
                uint64_t key = m_imageData.firstKey();
                if (m_imageTotalNum[key] == m_imageData[key].size())
                {
                    QMapIterator<int, QByteArray> it(m_imageData[key]);
                    while(it.hasNext())
                    {
                        it.next();
                        m_imageTotalData[key] += it.value();
                    }
                    emit s_this->DoParseImageData(key, m_imageTotalData[key]);
                    m_imageTotalData.clear();
                    m_imageData.clear();
                    m_imageTotalNum.clear();
                }
            }
            else
            {
                //qDebug() << "[getReoImageHandle]m_image size: " << m_imageData.size();
            }
        }
    }
}

void  MtcInterface::parseImageData(uint64_t tamps, QByteArray data)
{
    //    qDebug() << "[parseImageData]data: " << data << ", size: " << data.size();
    if (data.size() > 0) {
        int trackId  = data.at(0) & 0x000000FF;
        trackId |= ((data.at(1) <<8)&0x0000FF00);
        trackId |= ((data.at(2) <<16)&0x00FF0000);
        trackId  |= ((data.at(3)<<24)&0xFF000000);
        int reoIdLength = data.at(4) & 0x000000FF;
        reoIdLength |= ((data.at(5) <<8)&0x0000FF00);
        reoIdLength |= ((data.at(6) <<16)&0x00FF0000);
        reoIdLength  |= ((data.at(7)<<24)&0xFF000000);
        QByteArray reoData =  data.mid(8, reoIdLength);
        int rgbLen = data.at(8+reoIdLength) & 0x000000FF;
        rgbLen |= ((data.at(9+reoIdLength) <<8)&0x0000FF00);
        rgbLen |= ((data.at(10+reoIdLength) <<16)&0x00FF0000);
        rgbLen  |= ((data.at(11+reoIdLength)<<24)&0xFF000000);
        //        qDebug() << "[parseImageData]trackId: " << trackId << ", ReoIDLength:" << reoIdLength
        //                 << ", rgbLen: " << rgbLen;
        QByteArray rgbData = data.mid(12+reoIdLength, rgbLen);
        int rgbBackLen =  data.at(12+reoIdLength+rgbLen) & 0x000000FF;
        rgbBackLen |= ((data.at(13+reoIdLength+rgbLen) <<8)&0x0000FF00);
        rgbBackLen |= ((data.at(14+reoIdLength+rgbLen) <<16)&0x00FF0000);
        rgbBackLen  |= ((data.at(15+reoIdLength+rgbLen)<<24)&0xFF000000);
        QByteArray rgbBackData = data.mid(16+reoIdLength+ rgbLen, rgbBackLen);
//        qDebug() << "[parseImageData]trackId: " << trackId << ", ReoIDLength:" << reoIdLength
//                 << ", rgbLen: " << rgbLen << ", rgbBackLen:" <<rgbBackLen
//                 << ", total: " << 8+reoIdLength+8+rgbLen+rgbBackLen;
        if (reoIdLength+rgbLen+ rgbBackLen <0) {
            return;
        }
        if (data.size() > 8+reoIdLength+8+rgbLen+rgbBackLen) {
            //            qDebug() << "[parseImageData]has IR data";
            int irLen =  data.at(16+reoIdLength+rgbLen+rgbBackLen) & 0x000000FF;
            irLen |= ((data.at(17+reoIdLength+ rgbLen+rgbBackLen) <<8)&0x0000FF00);
            irLen |= ((data.at(18+reoIdLength+ rgbLen+rgbBackLen) <<16)&0x00FF0000);
            irLen |= ((data.at(19+reoIdLength+ rgbLen+rgbBackLen)<<24)&0xFF000000);
            QByteArray irData = data.mid(20 +reoIdLength+ rgbLen+rgbBackLen, irLen);
            QFile file(QString("image/%1_%2_%3").arg(tamps).arg(trackId).arg("imageFaceBackIR.jpg"));
            file.open(QIODevice::WriteOnly);
            file.write(irData);
            file.flush();
            file.close();
        }

        QFile file2(QString("image/%1_%2_%3").arg(tamps).arg(trackId).arg("imageFaceBackRGB.jpg"));//实际上目前设备上报该图是与imageFaceRGB一样的
        file2.open(QIODevice::WriteOnly);
        file2.write(rgbBackData);
        file2.flush();
        file2.close();
    }
}

void MtcInterface::getReoFeatureHandle(char * value, int len)
{
    QMutexLocker locker(&s_this->m_mutexGetReoFeature);
    if(len > 4)
    {
        QByteArray tmp(value, len);
        int traceIdLen = tmp.at(0) & 0x000000FF;
        traceIdLen |= ((tmp.at(1) << 8)&0x0000FF00);
        traceIdLen |= ((tmp.at(2) << 16)&0x00FF0000);
        traceIdLen |= ((tmp.at(3) << 24)&0xFF000000);//recoIdLength
        if(traceIdLen > 0 && len > (4+traceIdLen+4))
        {
            QByteArray traceId(value+4, traceIdLen);//recoId
            int featureLen = tmp.at(4+traceIdLen) & 0x000000FF;
            featureLen |= ((tmp.at(5+traceIdLen) << 8)  & 0x0000FF00);
            featureLen |= ((tmp.at(6+traceIdLen) << 16) & 0x00FF0000);
            featureLen |= ((tmp.at(7+traceIdLen) << 24) & 0xFF000000);//featureLength
            if(featureLen > 0 && len >= (4+traceIdLen+4+featureLen))
            {
                QByteArray Feature(value+4+traceIdLen+4, featureLen);//feature
                QFile file(QString("image/%1_%2.feature").arg(traceId.data()).arg(QString::number(QDateTime::currentMSecsSinceEpoch() / 1000)));
                file.open(QIODevice::WriteOnly);
                file.write(Feature);
                file.close();
            }
        }
    }
}

void MtcInterface::getQRCodeReportHandle(char * value, int len)
{
    if(len > 4)
    {
        QByteArray tmp(value, len);
        uint length  = tmp.at(0) & 0x000000FF;
        length |= ((tmp.at(1) <<8)&0x0000FF00);
        length |= ((tmp.at(2) <<16)&0x00FF0000);
        length |= ((tmp.at(3)<<24)&0xFF000000);
        //qDebug() << "[getQRCodeReportHandle]length: "<<length << ", len: " << len;
        if(length == len - 4)
        {
            QByteArray qrCodeJson = tmp.mid(4, length);
            emit s_this->qrCodeReportSignal(qrCodeJson);
        }else
        {
            //qDebug() << "[getQRCodeReportHandle] value size no suit.";
        }
    }
}

int MtcInterface::mtc_closeAutoReport()
{
    int result = -1;
    result = CloseAutoUploadAiInfo(dev);
    emit this->closeAutoReportResult(result);

    if(m_parseIDThread != nullptr)
    {
        m_parseIDThread->deleteLater();
        m_parseIDThread = nullptr;
    }
    if(m_thread != nullptr)
    {
        m_thread->quit();
        m_thread->wait(3000);
        m_thread->deleteLater();
        m_thread = nullptr;
    }
    return result;
}

int MtcInterface::upgradeUpload_thread(QString filePath)
{
    //    QByteArray data;
    //    int ret = 0;
    //    int slices = 0;
    //    int position = 0;

    //    QFile file(filePath);
    //    if (!file.open(QIODevice::ReadOnly))
    //    {
    //        return -1;
    //    }
    //    data = file.readAll();
    //    file.close();
    //    if(data.length() > 0)
    //    {
    //        slices = uploadPackage(dev,0, data.data(), data.length());
    //        data.clear();
    //        emit this->setUploadRange(slices);
    //        if(slices <= 0)
    //        {
    //            ret = -3;
    //        } else
    //        {
    //            while (position < slices)
    //            {
    //                if(m_isCancelUpgrade)
    //                {
    //                    cancelUpload(dev);
    //                    m_isCancelUpgrade = false;
    //                }
    //                position = checkUploadState(dev);
    //                if(position == -1){return -1;}
    //                emit this->uploadPackageProgress(position);
    //                QThread::msleep(500);
    //            }
    //        }
    //    }

    //    printf("upgrade_test ret = %d.\n", ret);
    //    return ret;
    return -1;
}

int MtcInterface::upgradeUpload(QString filePath)
{
    char data[MTC_UPGRADE_SEGMENT_LENGTH] = {0};
    int length = 0;
    int position = 0;
    int ret = 0;
    m_isCancelUpgrade = false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    while (!file.atEnd())
    {
        if(m_isCancelUpgrade)
        {
            break;
        }
        length = file.read(data, MTC_UPGRADE_SEGMENT_LENGTH);
        if(length <= 0)
        {
            break;
        }

        if(UploadPackageSlice(dev,position, data, length) != 0)
        {
            ret = -3;
            break;
        }
        position += length;
        //qDebug() << "upgradeUpload position : " << position;
        emit this->uploadPackageProgress(position);
    }
    file.close();
    printf("upgrade_test ret = %d.\n", ret);
    return ret;
}

int MtcInterface::upgradeApply(QString filePath)
{
    char md5_result[1024] = {0};
    int length = 32;

    QByteArray md5Byte = getFileMd5(filePath);
    if(md5Byte.isEmpty()){
        emit this->uploadStatus(false,tr("get md5 value failed.\n"),false);
        return -1;
    }
    md5_result[32] = '\0';
    //qDebug() << "upgrade md5 " << md5Byte << "  Hex: " << md5Byte.toHex() << "  length is " << md5Byte.length() << "    hex length is " << md5Byte.toHex().length();

    QByteArray md5hex = md5Byte.toHex();
    if(applyUpgradePackage(md5hex.data(), md5hex.length()) != 0)
    {
        return -1;
    }
    return 0;
}

QByteArray MtcInterface::getFileMd5(QString filePath)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly))
    {
        //qDebug() << "file open error.";
        return 0;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    localFile.close();
    QByteArray md5 = ch.result();
    return md5;
}

void MtcInterface::startRecognize(int mode)
{
    int result = -1;
    result = ResumeRecognize(dev);
    emit this->startRecognizeResult(result);
}

void MtcInterface::stopRecognize(int mode)
{
    int result = -1;
    result = PauseRecognize(dev);
    emit this->stopRecognizeResult(result);
}

void MtcInterface::QueryRecognizeStatus()
{
    int result = -1;
    result = QueryRecognize(dev);
    emit this->queryRecognizeStatusResult(result);
}

void MtcInterface::getVersion()
{
    char buffer[1024];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0, buf_size);
    GetDeviceVersion(dev, buffer, buf_size);
    emit this->getVersionResult(buffer);
}

void MtcInterface::getSingleRecognize()
{
    int result = -1;
    char buffer[1024];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0, buf_size);
    result = GetSingleRecognize(dev, buffer, buf_size);
    emit this->getSingleRecognizeResult(result,buffer);
}

void MtcInterface::getSN()
{
    char buffer[128];
    char buffer1[128];
    char buffer2[128];
    memset(buffer,  0, sizeof(buffer));
    memset(buffer1, 0, sizeof(buffer1));
    memset(buffer2, 0, sizeof(buffer2));

    char device_sn_mode = 0;
    int ret = GetDevSn(dev, device_sn_mode, buffer1, sizeof(buffer1));
    if(ret == 0)
    {
        strcat(buffer, buffer1);
        strcat(buffer, "\n");
    }

    device_sn_mode = 1;
    ret = GetDevSn(dev, device_sn_mode, buffer2, sizeof(buffer2));
    if(ret == 0)
    {
        strcat(buffer,buffer2);
    }

    emit this->getSnResult(buffer);
}

void MtcInterface::getModel()
{
    char buffer[128];
    uint buf_size = sizeof(buffer);
    memset(buffer, 0, buf_size);
    GetDevModel(dev, buffer, buf_size);
    emit this->getModelResult(buffer);

}

void MtcInterface::getLibrary()
{   
    uint nFaceNum = 0;
    int ret = GetDeviceFaceLibraryNum(dev, &nFaceNum);
    if(0 == ret)
    {        
        emit this->getLibraryResult(nFaceNum);
    }else
    {
        //qDebug() << "get library number is false!";
        emit this->getLibraryResult(-1);
    }

}
void MtcInterface::switchCameraRGB()
{
    int result = -1;
    result = SwitchCamRgbIr(dev, MTC_SWITCH_RGB);
    emit this->switchCameraRgbResult(result);
}
void MtcInterface::switchCameraIR()
{
    int result = -1;
    result = SwitchCamRgbIr(dev, MTC_SWITCH_IR);
    emit this->switchCameraIrResult(result);
}
