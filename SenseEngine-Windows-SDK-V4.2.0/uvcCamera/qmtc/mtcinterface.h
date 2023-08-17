/************************************************************************
Copyright  : 2018-2028,Shenzhen SenseTime Co.,Ltd
File name  : %FILENAME%
Description: .
Author     : longzeyuan
Date       : 2019-07-23
Version    : V1.0.3
History    : 2019-07-23, Create it by longzeyuan
************************************************************************/
#ifndef MTCINTERFACE_H
#define MTCINTERFACE_H

#include <QObject>

#include <QMutex>
//#include <QSerialPort>
#include <QMap>
#include"parseimagedatathread.h"
#include "stdint.h"
#include "globaltype.h"

#define MTC_TAG_ADD_IMAGE		0x0001
#define MTC_TAG_DEL_IMAGE		0x0002
#define MTC_TAG_QUERY_IMAGE		0x0003
#define MTC_TAG_INPUT_IMAGE     0x0004
#define MTC_TAG_ADD_FEATURE     0x0005
#define MTC_TAG_QUERY_FEATURE   0x0006

#define MTC_TAG_START_RECOGNIZE		0x0010
#define MTC_TAG_GET_RECOGNIZE_RECORD	0x0011
#define MTC_TAG_STOP_RECOGNIZE		0x0012
#define MTC_TAG_START_ONETOENE_RECOGNIZE		0x0013
#define MTC_TAG_RESUME_RECOGNIZE		0x0014
#define MTC_TAG_PAUSE_RECOGNIZE		0x0015
#define MTC_TAG_QUERY_RECOGNIZE		0x0016
#define MTC_TAG_SET_REOCONFIG       0x0017
#define MTC_TAG_AI_CONTROL_UPLOAD_INFO       0x0018
#define MTC_TAG_AI_CONTROL_FRAME_PRIVATE     0x0019
#define MTC_TAG_UPLOAD_UPGRADE_PACKAGE	0x0020
#define MTC_TAG_APPLY_UPGRADE_PACKAGE	0x0021

#define MTC_TAG_AI_TRACK_UPLOAD              0x0030
#define MTC_TAG_AI_VERIFY_UPLOAD             0x0031
#define MTC_TAG_AI_IMAGE_UPLOAD              0x0033
#define MTC_TAG_AI_FEATURE_UPLOAD            0x0034
#define MTC_TAG_GET_LIBRARY_COUNT 0x0035

#define MTC_TAG_PING_TEST		0x1000
#define MTC_TAG_SHELL_CMD		0x1001
#define MTC_TAG_COMMAND_MODE    0x1002
#define MTC_TAG_VERSION_INFO    0x1003
#define MTC_TAG_SYSTEM_REBOOT   0x1004
#define MTC_TAG_SYSTEM_RESET    0x1005

#define MTC_TAG_SN              0x2001
#define MTC_TAG_MODEL              0x2002
#define MTC_TAG_SWITCH_RGBIR    0x2005
#define MTC_TAG_IR_LIGHT        0x2006
#define MTC_TAG_FRAME           0x2007
#define MTC_TAG_LIGHT_SENSOR    0x200c

#define MTC_REBOOT              0x00 //reboot
#define MTC_REBOOT_USER_DISK    0x01 //user reboot with Udisk
#define MTC_REBOOT_USER_NODISK  0x02 //user reboot without Udisk
#define MTC_REBOOT_FACT_DISK    0x03 //factory reboot with Udisk
#define MTC_REBOOT_FACT_NODISK  0x04 //factory reboot without Udisk

#define MTC_ADD_IMAGE_LOCAL_CAPTURE	0x00 //Camera capture
#define MTC_ADD_IMAGE_REMOTE_INPUT	0x01 //face photo from host device
#define MTC_ADD_IMAGE_CAPTURE_RETURN 0x02 //Camera capture and return picture file
#define MTC_DEL_IMAGE_ALL		0x00
#define MTC_DEL_IMAGE_ONE		0x01
#define MTC_QUERY_IMAGE_BY_ID		0x00
#define MTC_QUERY_IMAGE_BY_IMAGE	0x01
#define MTC_FACE_RECOGNIZE_CONINUE	0x00
#define MTC_FACE_RECOGNIZE_SINGLE	0x01
#define MTC_SINGLE_MODE          0x00
#define MTC_MULTI_MODE         0x01
#define MTC_NOLIVING_MODE       0x00
#define MTC_LIVING_MODE         0x01
#define MTC_SWITCH_RGB          0x00
#define MTC_SWITCH_IR           0x01
#define MTC_QUERY_FRAME         0x00
#define MTC_SET_FRAME           0x01
#define MTC_GET_IR_LIGHT        0x01
#define MTC_SET_IR_LIGHT        0x00
#define REO_AI_CONFIG           0x00
#define DEV_OT_CONFIG           0x01
#define OPEN_CAMERA             0x00
#define CLOSE_CAMERA            0x01
#define RGB_CAMERA              0x00
#define IR_CAMERA               0x01

#define MTC_IMAGE_ID_LENGTH			32
#define MTC_FEATURE_ID_LENGTH			32
//#define MTC_UPGRADE_SEGMENT_LENGTH		4080
#define MTC_UPGRADE_SEGMENT_LENGTH		8080
/**
 * @brief The MtcInterface class
 */


typedef   QMap<uint64_t, QMap<int, QByteArray>> mapFunc;

class MtcInterface : public QObject
{
    Q_OBJECT

    typedef struct mtc_image
    {
        unsigned int image_length;
        unsigned char *image;
        unsigned int id_length;
        unsigned char id[MTC_IMAGE_ID_LENGTH];
    }mtc_image;

    typedef struct mtc_feature
    {
        unsigned int feature_length;
        unsigned char *feature;
        unsigned int id_length;
        unsigned char id[MTC_FEATURE_ID_LENGTH];
    }mtc_feature;

    typedef struct mtc_json
    {
        unsigned int json_length;
        unsigned char json[1024];
    }mtc_json;

public:
    static MtcInterface* s_this;

    explicit MtcInterface(QObject *parent = nullptr, void* dev = nullptr);
    ~MtcInterface();

    void* dev = nullptr;

public slots:
    void initDev();
    void setPortName(const QString &portName);

    void openSerialPort();
    void closeSerialPort();
    int mtc_openAutoUploadFaceInfoInFrame();
    int mtc_openAutoUploadFaceInfoInFrameResult();
    int mtc_closeAutoUploadFaceInfoInFrameResult();
    int mtc_antiFlickerSignalResult(int isEnable,int RGBIR,int HZ);
    int mtc_getAntiFlickerSignalResult(int RGBIR);
    int mtc_ping(QString data);
    bool check_model();
    void mtc_query_face(QString imageId);
    void _deleteImage(int mode,QString imageId);
    void _addImage(int mode ,QString imageId ,QString filePath);
    int tryAddImage(int mode ,QString imageId ,QString filePath);
    int tryAddImageSlice(QString imageId,QString filePath,uint fileLen);
	int inputLibrary(int mode, QString filePath);
    int imputLibrarySingle(int mode, QString filePath);
    int imputLibrarySlice(int mode, QString filePath);
    int mtcAddImage(int mode,QString imageId,QString filePath);
    int mtcAddFeature(QString featureId,QString filePath);
    void addImageBatch(QString filePath);
    void openCamera();
    void closeCamera();
    static void cancelAddImageBatch();
    static void cancelUpgradeBatch();
    void upgradeSlots(QString filePath);

    //int uploadPackage(int position, unsigned char *data, int length);
    int applyUpgradePackage(char *md5, int length);
    int mtcStartOnetoNumRecognize(int reomode, int mulmode);
    int mtc_start_onetoone_recognize(int reomode, int mulmode, QString filePath);
    int mtcStartO2ORecognizeSingle(int reomode, int mulmode, QString filePath);
    int mtcStartO2ORecognizeSlice(int reomode, int mulmode, QString filePath);
    int mtc_query_feature(QString imageId);
    int mtc_set_frame(int frame);
    int mtc_get_frame();
    int mtc_set_irlight(int irlight,int closeTime);
    int mtc_get_irlight();
    int mtc_set_logconfig(int logLever ,int logTarget);
    int mtc_get_logconfig();
    int mtc_set_RecognizeCount(int recognizeCount, int livingCount);
    int mtc_get_RecognizeCount();
    int mtc_set_QRCodeSwitch(int mode, int interval);
    int mtc_get_cpuTemperature();
    int mtc_get_aesensormodel();

    int mtc_open_TemlateUpdate();
    int mtc_close_TemlateUpdate();
    int mtc_get_TemlateUpdate();

    int mtc_set_aiconfig(QString aiconfig);
    int mtc_get_aiconfig();
    int mtc_get_lightsensor();
    int mtc_set_videosize(int size);
    int mtc_get_videosize();
    int mtc_set_screenmode(int mode);
    int mtc_set_rotation(int angle);
    int mtc_query_recognize(int mode);
    int mtc_reboot(int mode);
    int mtc_recovery(int mode);
    int mtc_opencamera(int mode);
    int mtc_closecamera(int mode);
    int mtc_getidlist();
    int mtc_openAutoReport(int imageMode);
    int mtc_closeAutoReport();

    int mtc_setUmodeToEngin();
    int mtc_reset();
    int mtc_setStreamFormat();
    int mtc_getModelAppVersion();

    void startRecognize(int mode);
    void stopRecognize(int mode);
    void QueryRecognizeStatus();
    void getVersion();
    void getSingleRecognize();
    void getSN();
    void getModel();
    void getLibrary();
    void switchCameraRGB();
    void switchCameraIR();
    static void getReoReportDataHandle(char *, int);
    static void getReoTrackHandle(char *, int);
    static void getReoImageHandle(char *, int);
    static void getReoFeatureHandle(char *, int);
    static void getQRCodeReportHandle(char *value, int len);
//    static void getCameraStatusHandle(char, char*, char);
//    static void getSerialStatusHandle(char, char*, char);

signals:
    void portStatusMsgSignal(const bool &status,const QString &msg);
    void errorSignal(const QString &s);
    void openFrameAutoResult(bool result);
    void closeFrameAutoResult(bool result);
    void setDeviceNoFlickerHzSignal(bool result);
    void getDeviceNoFlickerHzSignal(int result, int cameraId, int hz, int isEnable);
    void pingResult(bool result);
    void checkModelM20(DeviceType type,int frameWidth,int frameHeight);
    void queryFaceResult(int result, QString imageId);
    void deleteImageResult(int mode,int result);
    void addImageResult(int result);
    void inputLibraryResult(int result, QString verifyinfo);
    void addImageBatchStatus(int result,QString fileName);
    void uploadPackageProgress(int value);
    void setUploadRange(int value);
    void uploadStatus(bool isComplete,QString msg,bool isSuccess);
    void queryFeatureResult(int result);
    void getIDListResult(int result);
    void openAutoReportResult(int result);
    void closeAutoReportResult(int result);
    void startRecognizeResult(int result);
    void stopRecognizeResult(int result);
    void queryRecognizeStatusResult(int result);
    void onetooneRecognizeResult(int result);
    void setFrameResult(int result);
    void getFrameResult(int result, int frame);
    void setIrLightResult(int result);
    void getIrLightResult(int result, int light, int closeTime);
    void setLogConfigResult(int result);
    void getLogConfigResult(int result, int logLever ,int logTarget);
    void setQRCodeSwitchResult(int mode, int interval, int result);
    void getCpuTemperatureResult(int result, int temp);
    void getAESensorModelResult(int result, int sensorModel);

    void setRecognizeCountResult(int result);
    void getRecognizeCountResult(int result,int recognizeCount,int livingCount);

    void setTemlateUpdateResult(int result,int temlateUpdateMode);
    void getTemlateUpdateResult(int result,int temlateUpdateMode);

    void getLightSensorResult(int result, int light);
    void setVideoSizeResult(int result);
    void getVideoSizeResult(int result, int width, int height);
    void setScreenModeResult(int mode);
    void setRotateAngleResult(int angle);
    void rebootResult(int result);

    void setUmodeToEnginResult(int result);
    void resetResult(int result);
    void setStreamFormatResult(int result);
    void getModelAppVersionResult(int result, QString modelAppVersion);

    void recoveryResult(int result);
    void opencameraResult(int result);
    void closecameraResult(int result);
    void setAiConfigResult(int result);
    void getAiconfigResult(QString buffer);
    void getVersionResult(QString result);
    void getSingleRecognizeResult(int result,QString recInfo);
    void getSnResult(QString result);
    void getModelResult(QString result);
    void getLibraryResult(int result);
    void switchCameraRgbResult(int result);
    void switchCameraIrResult(int result);

    void DoParseImageData(unsigned long long tamps, QByteArray data);
    void qrCodeReportSignal(QString result);
private:
    int upgradeUpload_thread(QString filePath);
    int upgradeUpload(QString filePath);
    int upgradeApply(QString filePath);
    QByteArray getFileMd5(QString filePath);
    void  parseImageData(uint64_t tamps, QByteArray data);

private:
    QString m_portName="";
    QString m_pingData;
     QMutex m_mutexGetReoFeature;
    QByteArray m_imageFaceRGB = nullptr;
    QByteArray m_imageFaceBackRGB = nullptr;
    QByteArray m_imageBackIR = nullptr;
    static mapFunc m_imageData;
    static QMap<uint64_t, QByteArray> m_imageTotalData;
    static QMap<uint64_t, int> m_imageTotalNum;
    int m_allImageSliceCount = 0;
    int m_currentImageSlice = 0;
    static bool m_isCancel;
    static bool m_isCancelUpgrade;

    QThread *m_thread = nullptr;
    parseImageDataThread *m_parseIDThread = nullptr;
};

#endif // MTCINTERFACE_H
