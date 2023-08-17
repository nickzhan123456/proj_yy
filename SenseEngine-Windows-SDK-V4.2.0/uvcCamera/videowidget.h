#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QImage>
#include <QTimer>
#include "XFFmpeg.h"
#include <QPixmap>
#include <QMutex>
#include "globaltype.h"

#define DISPLAY_TIMEOUT 3000

class videoWidget :public QWidget
{
    Q_OBJECT
public:
    explicit videoWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);

public:
    virtual ~videoWidget();
public slots:
    void setCurrentResolution(int height,int width);
    void faceDataSlot(QList<XFFmpeg::FaceData> list);
    void receiveImageSlot(const QImage &image);
    void setDeviceType(DeviceType type);
    void liveScoreTimeout();
    void livenessTimeout();

private:
    void paintFaceRect(QPainter *painter);
    QImage getImage();

private:
    QImage *m_image = nullptr;
    int current_w = 0;
    int current_h = 0;
    int m_original_h = 0;
    int m_original_w = 0;
    float m_ration_face_h = 1.0;
    float m_ration_face_w = 1.0;

    int m_fontsize = 12;
    QTimer m_tmrLiveScore;
    QString m_liveScore;
    QTimer m_tmrLiveness;
    QString m_liveness;
    QFont  m_font;

    QMutex m_mutex;
    QImage m_img;
    DeviceType m_deviceType = DEVICE_UNKNOW;

    QList<XFFmpeg::FaceData> m_faceDataList;
};

#endif // VIDEOWIDGET_H
