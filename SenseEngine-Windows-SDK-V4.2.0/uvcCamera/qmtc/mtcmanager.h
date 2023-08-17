#ifndef MTCMANAGER_H
#define MTCMANAGER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include "mtcinterface.h"
#include "globaltype.h"

class MtcManager : public QObject
{
    Q_OBJECT
public:
    static MtcManager * instance();
    ~MtcManager();

    void open();
    void closePort();

    void openSerialPort(const QString &portName);
    void mtcOpenAutoUploadFaceInfoInFrame();
    void mtcCloseAutoUploadFaceInfoInFrame();
    void mtcPingTest(QString data);
    void mtcCheckModel();
    void queryFace(QString imageId);
    void deleteAll();
    void deleteOne(QString imageId);
    void addBaseLibraryByCamera(QString imageId);
    void addBaseLibraryByCameraReturn(QString imageId);
    void addImageFile(QString imageId,QString filePath);
    void inputLibrary(QString filePath = "");
    void addFeature(QString imageId, QString filePath);
    void queryFeature(QString featureId);
    void setFrame(int frame);
    void getFrame();
    void setIrLight(int irlight,int closeTime);
    void setLogConfig(int logLever ,int logTarget);
    void setRecognizeCount(int recognizeCount, int livingCount);
    void getRecognizeCount();
    void setQRCodeSwitch(int mode, int interval);
    void getCpuTemperature();
    void getAESensorModel();
    void getSingleRecognize();

    void openTemlateUpdate();
    void closeTemlateUpdate();
    void getTemlateUpdate();

    void addImageBatch(QString fileName);
    void closeCamera();
    void openCamera();
    void cancelAddImageBatch();
    void cancelUpgradeBatch();
    void upgrade(QString filePath);
    void StartRecognize(int mode);
    void StopRecognize(int mode);
    void QueryRecognizeStatus();
    void onetonumReomode(int reomode, int mulmode);
    void onetooneFile(int reomode, int mulmode, QString filePath);
    void GetVersion();    
    void getSN();
    void getModel();
    void GetLibrary();
    void GetIrLight();
    void GetLogConfig();
    void GetLightSensor();
    void setVideoSize(int size);
    void getVideoSize();
    void setScreenMode(int mode);
    void setRotation(int angle);
    void SetAiConfig(QString aiconfig);
    void GetAiConfig();
    void SwitchCameraRGB();
    void SwitchCameraIR();
    void OpenCamera(int mode);
    void CloseCamera(int mode);
    void GetIDList();
    void OpenAutoReport(int imageMode);
    void CloseAutoReport();
    void OpenFrameReport();
    void CloseFrameReport();
    void antiFlicker(int isEnable,int RGBIR,int HZ);
    void GetAntiFlicker(int RGBIR);
    void Reboot(int mode);
    void Recovery(int mode);

    void SetUmodeToEngin();
    void Reset();
    void SetStreamFormat();
    void GetModelAppVersion();

    void* GetDevHandle(){return m_mtcInterface->dev;}


signals:
    void errorSignal(const QString &s);
    void closeSerialPort();

    void closeFrameAutoResult(bool result);
    void openFrameAutoResult(bool result);
    void setDeviceNoFlickerHzSignal(bool result);
    void getDeviceNoFlickerHzSignal(int result, int cameraId, int hz, int isEnable);
    void pingResult(bool result);
    void checkModelM20(DeviceType type,int frameWidth,int frameHeight);

    void initDevSingnal();
    void setPortNameSingnal(const QString &portName);
    void startOpenPort();
    void portStatusMsgSignal(const bool &status,const QString &msg);  

    void mtcOpenAutoUploadFaceInfoInFrameSingnal();
    void mtcCloseAutoUploadFaceInfoInFrameSingnal();
    void mtcPintTestSignal(QString data);
    void check_modelSignal();
    void queryFaceSignal(QString imageId);
    void deleteImageSignal(int mode,QString imageId);    
    void queryFaceResult(int result, QString imageId);
    void deleteImageResult(int mode,int result);

    void addImageSignal(int mode,QString imageId,QString filePath);
    void inputLibrarySignal(int mode,QString filePath);
    void addFeatureSignal(QString imageId, QString filePath);
    void queryFeatureSignal(QString featureId);
    void setFrameSignal(int frame);
    void getFrameSignal();
    void setIrLightSignal(int irlight,int closeTime);    
    void getIrLightSignal();
    void setLogConfigSignal(int logLever ,int logTarget);
    void getLogConfigSignal();
    void setRecognizeCountSingnal(int recognizeCount,int livingCount);
    void getRecognizeCountSingnal();
    void setQRCodeSwitchSingnal(int mode, int interval);
    void qrCodeReportSignal(QString result);
    void getCpuTemperatureSignal();
    void getAESensorModelSignal();
    void getCpuTemperatureResult(int result,int temp);
    void getAESensorModelResult(int result, int sensorModel);

    void openTemlateUpdateSingnal();
    void closeTemlateUpdateSingnal();
    void getTemlateUpdateSingnal();

    void getLightSensorSignal();
    void setVideoSizeSignal(int size);
    void getVideoSizeSignal();
    void setScreenModeSignal(int mode);
    void setRotationSignal(int angle);
    void setAiConfigSignal(QString aiconfig);
    void getAiConfigSignal();
    void addImageResult(int result);
    void inputLibraryResult(int result, QString verifyinfo);
    void addImageBatchSignal(QString fileName);
    void closeCameraSignal();
    void openCameraSignal();
    void addImageBatchResultSignal(int result,QString fileName);

    void upgradeSignal(QString filePath);
    void uploadPackageProgress(int value);
    void setUploadRange(int value);
    void uploadStatus(bool isComplete,QString msg,bool isSuccess);
    void mtcStartRecognizeSignal(int mode);
    void mtcStopRecognizeSignal(int mode);
    void mtcQueryRecognizeStatusSignal();
    void onetonumReomodeSignal(int reomode, int mulmode);
    void startOnetooneSignal(int reomode, int mulmode, QString filePath);
    void mtcGetVersionSignal();
    void mtcGetSingleRecognizeSignal();

    void mtcgetSNSignal();
    void mtcgetModelSignal();
    void mtcGetLibrarySignal();
    void mtcSwitchCameraRGBSignal();
    void mtcSwitchCameraIRSignal();
    void OpenCameraSignal(int mode);
    void CloseCameraSignal(int mode);
    void GetIDListSignal();
    void OpenAutoReportSignal(int imageMode);
    void CloseAutoReportSignal();
    void OpenFrameReportSignal();
    void CloseFrameReportSignal();
    void antiFlickerSignal(int isEnable,int RGBIR,int HZ);
    void GetAntiFlickerSignal(int RGBIR);
    void RebootSignal(int mode);
    void RecoverySignal(int mode);

    void SetUmodeToEnginSignal();
    void ResetSignal();
    void SetStreamFormatSignal();
    void GetModelAppVersionSignal();

    void startRecognizeResult(int result);
    void stopRecognizeResult(int result);       
    void queryRecognizeStatusResult(int result);
    void onetooneRecognizeResult(int result);
    void queryFeatureResult(int result);
    void getIDListResult(int result);
    void openAutoReportResult(int result);
    void closeAutoReportResult(int result);
    void setFrameResult(int result);
    void getFrameResult(int result, int frame);
    void setIrLightResult(int result);
    void getIrLightResult(int result, int lightValue, int closeTime);
    void setLogConfigResult(int result);
    void getLogConfigResult(int result, int logLever, int logTarget);
    void setRecognizeCountResult(int result);
    void getRecognizeCountResult(int result,int recognizeCount,int livingCount);
    void setQRCodeSwitchResult(int mode, int interval, int result);

    void setTemlateUpdateResult(int result, int temlateUpdateMode);
    void getTemlateUpdateResult(int result,int temlateUpdateMode);

    void getLightSensorResult(int result, int light);
    void setVideoSizeResult(int result);
    void getVideoSizeResult(int result, int width, int height);
    void setScreenModeResult(int result);
    void setRotateAngleResult(int angle);
    void setAiConfigResult(int result);
    void getAiConfigResult(QString result);
    void getVersionResult(QString result);    
    void getSingleRecognizeResult(int result,QString recInfo);
    void getSnResult(QString result);
    void getModelResult(QString result);
    void getLibraryResult(int result);
    void switchCameraRgbResult(int result);
    void switchCameraIrResult(int result);
    void rebootResult(int result);
    void recoveryResult(int result);
    void opencameraResult(int result);
    void closecameraResult(int result);

    void setUmodeToEnginResult(int result);
    void resetResult(int result);
    void setStreamFormatResult(int result);
    void getModelAppVersionResult(int result, QString modelAppVersion);

private:
    MtcManager(QObject *parent = nullptr);
    MtcManager(const MtcManager &);
    MtcManager &operator=(const MtcManager &);    

    static QMutex s_mutex;//Instance mutex
    static MtcManager *m_pInstance;

    MtcInterface *m_mtcInterface = nullptr;
    bool mtcInterface_connected = false;
    QThread *m_thread = nullptr;
    QString m_pingData;
};

#endif // MTCMANAGER_H
