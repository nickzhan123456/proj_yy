#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QCameraInfo>
#include <QLabel>
#include "qmtc/mtcinterface.h"
#include "globaltype.h"
#include <QTimer>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum RESULT_LEVEL
    {
        ERROR_LEVEL = 0,
        INFORM_LEVEL
    };
    enum ADDIMAGETYPE{
        UNKNOW_TYPE = 0,
        LOCAL_CAPTURE_TYPE,
        REMOTE_INPUT_TYPE
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showStatusMessage(const bool &status,const QString &message);
    void settingWidget();
    void disConnectionDev();
    void appAboutSlot();
    void StartRecognize();
    void StopRecognize();
    void QueryRecognizeStatus();
    void MultiLivingMode();
    void MultiNolivingMode();
    void SingleLivingMode();
    void SingleNolivingMode();
    void OtoMultiLivingMode();
    void OtoMultiNolivingMode();
    void OtoSingleLivingMode();
    void OtoSingleNolivingMode();
    void GetVersion();
    void getSN();
    void getModel();
    void GetLibrary();
    void SwitchCameraRGB();
    void SwitchCameraIR();
    void openRGBCamera(int mode);
    void closeRGBCamera(int mode);
    void openIRCamera(int mode);
    void closeIRCamera(int mode);
    void getIDList();
    void openAutoReport_faceRGB_backRGB_IR();
    void openAutoReport_faceRGB_backRGB();
    void openAutoReport_faceRGB_IR();
    void openAutoReport_backRGB_IR();
    void openAutoReport_faceRGB();
    void openAutoReport_backRGB();
    void openAutoReport_IR();
    void openAutoReport_NoImage();

    void closeAutoReport();
    void openFrameReport();
    void closeFrameReport();

    void antiFlickerEnableRGB50();
    void antiFlickerEnableRGB60();
    void antiFlickerEnableIR50();
    void antiFlickerEnableIR60();

    void antiFlickerDisenableRGB50();
    void antiFlickerDisenableRGB60();
    void antiFlickerDisenableIR50();
    void antiFlickerDisenableIR60();

    void antiFlickerRGBCamera();
    void antiFlickerIRCamera();

    void addBaseLibraryByCamera();
    void addBaseLibraryByCamera_return();
    void addBaseLibrary();
    void StaticCompareLocalCapture();
    void StaticCompareRemoteInput();
    void addFeature();
    void queryFeature();
    void addBatch();
    void setFrame();
    void getIrLight();
    void getLogConfig();
    void setRecognizeCount();
    void setRecognizeCountDispose(int recognizeCount,int livingCount);
    void getRecognizeCount();
    void setSwitchQRCode();
    void qrCodeReportSlot(QString result);
    void getCpuTemperature();
    void getAESensorModel();
    void getCpuTemperatureSlot(int result, int temp);
    void getAESensorModelSlot(int result, int sensorModel);
    void getSingleRecognize();

    void openTemlateUpdate();
    void closeTemlateUpdate();
    void getTemlateUpdate();

    void getAiConfig();
    void getLightSensor();
    void setVideoSize();
    void setScreenMode();
    void setRotation();
    void querySingle();
    void deleteOne();
    void deleteAll();
    void upgrade();
    void showReceviceResult(QString title,QString msg,RESULT_LEVEL level);
    void errorSlot(QString msg);
    void openFrameAutoResult(bool result);
    void closeFrameAutoResult(bool result);
    void setDeviceNoFlickerHzSignalResult(bool result);
    void getDeviceNoFlickerHzSignalResult(int result, int cameraId, int hz, int isEnable);
    void pingResult(bool result);
    void checkModelM20(DeviceType type,int frameWidth,int frameHeight);
    void setDeviceType(DeviceType type);
    DeviceType getDeviceType();
    void queryFaceResultSlot(int result, QString imageId);
    void deleteImageResultSlot(int mode,int result);
    void inputLibraryResultSlot(int result, QString verifyinfo);
    void addImageResultSlot(int result);
    void startRecognizeResultSlot(int result);
    void stopRecognizeResultSlot(int result);
    void queryRecognizeStatusResultSlot(int result);
    void onetooneRecognizeResultSlot(int result);
    void queryFeatureResultSlot(int result);
    void getIDListResultSlot(int result);
    void openAutoReportResultSlot(int result);
    void closeAutoReportResultSlot(int result);
    void setFrameResultSlot(int result);
    void getFrameResultSlot(int result, int frame);
    void setIrLightResultSlot(int result);
    void getIrLightResultSlot(int result, int lightValue, int closeTime);
    void setLogConfigResultSlot(int result);
    void getLogConfigResultSlot(int result, int logLever ,int logTarget);
    void setRecognizeCountResultSlot(int result);
    void getRecognizeCountResultSlot(int result,int recognizeCount,int livingCount);
    void setQRCodeSwitchResultSlot(int mode, int interval, int result);

    void setTemlateUpdateResultSlot(int result,int temlateUpdateMode);
    void getTemlateUpdateResultSlot(int result,int temlateUpdateMode);

    void getLightSensorResultSlot(int result, int light);
    void setVideoSizeResultSlot(int result);
    void getVideoSizeResultSlot(int result, int width, int height);
    void setScreenModeResultSlot(int result);
    void setRotateAngleResultSlot(int result);
    void setAiConfigResultSlot(int result);
    void getAiConfigResultSlot(QString result);
    void getVersionResultSlot(QString result);
    void getSingleRecognizeResultSlot(int result,QString recInfo);
    void getSnResultSlot(QString result);
    void getModelResultSlot(QString result);
    void getLibraryResult(int result);
    void switchCameraRgbResultSlot(int result);
    void switchCameraIrResultSlot(int result);
    void setCamera(QString cameraName,QString serialPort);
    void upgradeSuccess();
    void reboot();
    void recovery();
    void rebootResultSlot(int result);
    void recoveryResultSlot(int result);
    void opencameraResultSlot(int result);
    void closecameraResultSlot(int result);

    void setUmodeToEnginResultSlot(int result);
    void resetResultSlot(int result);
    void setStreamFormatResultSlot(int result);
    void getModelAppVersionResultSlot(int result, QString modelAppVersion);

    void userRebootWithDisk();
    void userRebootWithoutDisk();
    void factoryRebootWithDisk();
    void factoryRebootWithoutDisk();
    void setIsMirroredTrue();
    void setIsMirroredFalse();
    void msgLabelTimerOut();

    void setUmodeToEngin();
    void reset();
    void setStreamFormat();
    void getModelAppVersion();
    int compareVersion(QString version1, QString version2);

private:
    void setActionEnable(bool isEnable);

private:
    Ui::MainWindow *ui;
    QLabel *m_serailStatus = nullptr;
    QLabel *m_cameraStatus = nullptr;
    QLabel *m_devReopenTip = nullptr;
    QTimer m_msgTimer;

    QString message;
    QString m_cameraName;
    quint16 blockSize;
    bool m_showMsgDialog = false;
    int m_qrcodeStatus = 0;//0：OFF，1：ON
    int m_qrcodeInterval = 2;
    QFile *m_qrcode_file = nullptr;

    bool m_isInitCamera = false;
    bool m_isOpenRGB    = false;
    bool m_isGetVersion = false;
    DeviceType m_deviceType = DEVICE_UNKNOW;
    QString m_versionInfo;
};

#endif // MAINWINDOW_H
