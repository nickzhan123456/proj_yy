#pragma once
#include <QMutex>
#include <QList>
#include <QObject>
#include <QFile>
#include <QQueue>
class XFFmpeg :public QObject
{
    Q_OBJECT

public:
    static XFFmpeg *Get();
    void Setup(void* dev);

    struct FaceData{
        uint status;//recognition_status
        QString mask;//mask_status
        QString hat;//safety_hat
        QString uid;//it can be a ID, UNKNOW，STRANGER，NONLIVENESS,LIVENESS
        QString gender; //gender(0: woman 1：man)
        QString age;//age
        QString livescore;//livescore,only displayed if FR succeeds

        int top;//face coordinates
        int left;//face coordinates
        int right;//face coordinates
        int bottom;//face coordinates
    };

    int Open(QString deviceName);
    QString getDeviceName();
	void Close();

    int parseFrame();
    void parseVerifyData(char* data);
    void setFaceRectEnable(bool enable);
    void* dev = nullptr;

	virtual ~XFFmpeg();

signals:
    void currentResolution(int height,int width);
    void faceDataSignal(QList<FaceData> list);
    void showView();
    void receiveImage(const QImage &img);

public:
	bool isPlay = false;

private:
    XFFmpeg();
    XFFmpeg(const XFFmpeg &);//Disallow copying constructors.
    XFFmpeg & operator=(const XFFmpeg &);//Disallow assignment copy functions
    static QMutex s_mutex;//Instance mutex
    static XFFmpeg *m_pInstance;
    char errorbuf[1024];
    bool m_isPaintFaceRect = true;
    QMutex mutex;
    QString cameraName;

    bool m_isMirrored = false;
    int MirroredWidth = 0;
    bool m_isFrameReport = true;
    QByteArray m_jsonData;
    int m_jsonDataCount = 0;
    int m_jsonDataCountNow = 0;
    int m_parseVerifyDataCount = 0;
public:
    void setIsMirrored(bool isMirrored);
    void setIsFrameReport(bool IsFrameReport);
    void setJsonData(QByteArray jsonData);
};

