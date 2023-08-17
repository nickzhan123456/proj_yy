#include "videowidget.h"
#include <QDebug>
#include <QPainter>
#include "XVideoThread.h"
#include "XFFmpeg.h"
#include <QTime>
#include <QMutexLocker>

videoWidget::videoWidget(QWidget *parent) :
    QWidget(parent)
{
#ifdef _WIN32
    m_fontsize = 12;
#else
    m_fontsize = 10;
#endif
    m_font.setPointSize(m_fontsize);

    connect(XFFmpeg::Get(),&XFFmpeg::currentResolution,this,&videoWidget::setCurrentResolution);
    connect(XFFmpeg::Get(),&XFFmpeg::faceDataSignal,this,&videoWidget::faceDataSlot);
    connect(XFFmpeg::Get(),&XFFmpeg::receiveImage,this,&videoWidget::receiveImageSlot);
    connect(&m_tmrLiveScore, SIGNAL(timeout()), this, SLOT(liveScoreTimeout()));    
    connect(&m_tmrLiveness,  SIGNAL(timeout()), this, SLOT(livenessTimeout()));
    m_tmrLiveScore.stop();
    m_tmrLiveness.stop();
}

void videoWidget::paintEvent(QPaintEvent *e)
{
    QImage img = this->getImage();
    if(!img.isNull())
    {
        QPixmap pixmap(size());
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);

        static int w = 0;
        static int h = 0;
        if (w != width() || h != height())
        {
            w = width();
            h = height();
            double viewScale = (double)width() / (double)height();
            double videoScale = (double)m_original_w / (double)m_original_h;
            double aspectDeformation  = videoScale / viewScale -1;
            if (aspectDeformation > 0) {//If this is greater than 0, the video source aspect ratio is greater than the control aspect ratio,
                /*Smaller window width, fixed width, reduced height*/
                double imgScale = (double)m_original_h / (double)m_original_w;
                current_h = width()* imgScale;
                current_w  = width();
            } else {//If this is greater than 0, the video source aspect ratio is greater than the control aspect ratio,
                /*Smaller window width, fixed width, reduced height*/
                double imgScale = (double)m_original_w / (double)m_original_h;
                current_h = height();
                current_w  = height()* imgScale;
            }
        }

        int x=0,y=0;
        if(current_w <= width())
        {
            x = (width() - current_w)/2;
        }
        if(current_h <= height())
        {
            y = (height() - current_h)/2;
        }

        QRectF source = QRectF(QPointF(0, 0), img.size());
        QSize targetSize = QSize(current_w, current_h);
        QRectF target = QRectF(QPointF(x, y), targetSize);

        painter.drawImage(target, img, source);

        if(!m_faceDataList.isEmpty()){
            painter.save();
            painter.setPen(QPen(Qt::red,3));
            painter.translate(x,y);
            paintFaceRect(&painter);
            painter.restore();
        }

        painter.end();

        painter.begin(this);
        painter.drawPixmap(QPoint(0,0),pixmap);
    }
}

void videoWidget::paintFaceRect(QPainter *painter)
{
    painter->setFont(m_font);
    if(!m_faceDataList.isEmpty())
    {
        float ration_h = ((float)current_h / (float)m_original_h) * m_ration_face_h;
        float ration_w = ((float)current_w / (float)m_original_w) * m_ration_face_w;
        //qDebug() << "current_h is " << current_h << "m_original_h is " << m_original_h;
        //qDebug() << "ration is " << ration;
        foreach (XFFmpeg::FaceData face, m_faceDataList)
        {
            //qDebug() << "face.left " << face.left << "  face.top " << face.top << " face.right " << face.right << " face.bottom " << face.bottom;
            //Draw faces box
            QRectF rectangle(QPointF(face.left* ration_w, face.top* ration_h), QPointF(face.right* ration_w, face.bottom* ration_h));
            painter->setPen(QPen(Qt::red,3));
            painter->drawRect(rectangle);

            if(!face.mask.isEmpty())
            {
//                switch (m_deviceType) {
//                case DEVICE_M10:
//                case DEVICE_M20:
//                case DEVICE_M20S:
//                {
                    QString maskWarning = "please take off your mask!";
                    painter->save();
                    painter->setPen(Qt::red);
                    QFontMetrics fm = painter->fontMetrics();
                    painter->translate(QPointF(0,0));
                    //painter->fillRect(QRectF((current_w-fm.width(maskWarning))/2,4,fm.width(maskWarning),fm.height()),QBrush(QColor(255,255,255,200)));
                    painter->drawText(QPointF((current_w-fm.width(maskWarning))/2,fm.height()), maskWarning);
                    painter->restore();
//                    break;
//                }
//                case DEVICE_UNKNOW:
//                default:
//                    break;
//                }
            }

            if(!face.hat.isEmpty())
            {
//                switch (m_deviceType) {
//                case DEVICE_M10:
//                case DEVICE_M20:
//                case DEVICE_M20S:
//                {
                    QString hatWarning = "please take off your safety hat!";
                    painter->save();
                    painter->setPen(Qt::red);
                    QFontMetrics fm = painter->fontMetrics();
                    painter->translate(QPointF(0,m_fontsize*2));
                    //painter->fillRect(QRectF((current_w-fm.width(hatWarning))/2,4,fm.width(hatWarning),fm.height()),QBrush(QColor(255,255,255,200)));
                    painter->drawText(QPointF((current_w-fm.width(hatWarning))/2,fm.height()), hatWarning);
                    painter->restore();
//                    break;
//                }
//                case DEVICE_UNKNOW:
//                default:
//                    break;
//                }
            }

            if(face.status != 0)
            {
                int filedCount = 0;
                int filedStart = 1;
                int filedMaxLength = 0;
                QString maxLengthMsg;
                //Set the font

                QFontMetrics fm = painter->fontMetrics();

                if(face.uid.size() || m_tmrLiveness.isActive())
                {
                    filedCount++;
                    if(face.uid.size() == 0 && m_liveness.size()) face.uid = m_liveness;
                    filedMaxLength = face.uid.size();
                    maxLengthMsg   = face.uid;
                }

                if(face.age.size())
                {
                    filedCount++;
                    if(face.age.size() > filedMaxLength)
                    {
                        filedMaxLength = face.age.size();
                        maxLengthMsg = face.age;
                    }
                }

                if(face.gender.size())
                {
                    filedCount++;
                    if(face.gender.size() > filedMaxLength)
                    {
                        filedMaxLength = face.gender.size();
                        maxLengthMsg = face.gender;
                    }
                }

                //Set live value text
                if(face.livescore.size() == 0)//No liveness values were obtained
                {
                    if(m_tmrLiveScore.isActive())
                    {
                        painter->save();
                        painter->translate(QPointF(0,m_fontsize*4));
                        //painter->fillRect(QRectF((current_w-fm.width(m_liveScore))/2,4,fm.width(m_liveScore),fm.height()),QBrush(QColor(255,255,255,200)));
                        painter->setPen(Qt::red);
                        painter->drawText(QPointF((current_w-fm.width(m_liveScore))/2,fm.height()), m_liveScore);
                        painter->restore();
                    }
                }
                else//Got the liveness value
                {
                    m_liveScore = face.livescore;
                    m_tmrLiveScore.start(DISPLAY_TIMEOUT);
                    painter->save();
                    painter->translate(QPointF(0,m_fontsize));
                    //painter->fillRect(QRectF((current_w-fm.width(m_liveScore))/2,4,fm.width(m_liveScore),fm.height()),QBrush(QColor(255,255,255,200)));
                    painter->setPen(Qt::red);
                    painter->drawText(QPointF((current_w-fm.width(m_liveScore))/2,fm.height()), m_liveScore);
                    painter->restore();
                }

                if(filedCount > 0)
                {
                    //Set text Background
                    painter->save();
                    painter->translate(QPointF((face.left + (face.right-face.left)/2)* ration_w - fm.width(maxLengthMsg)/2 - 2,(face.top)* ration_h - fm.height() - (filedCount*(m_fontsize)) + 3));
                    //painter->fillRect(QRectF(0,4,fm.width(maxLengthMsg),(fm.height() - 3)*filedCount + 5),QBrush(QColor(255,255,255,200)));
                    painter->restore();

                    //Set the face library ID
                    if(face.uid.size() == 0)
                    {
                        if(m_tmrLiveness.isActive())
                        {
                            painter->save();
                            painter->setPen(Qt::red);
                            painter->drawText(QPointF((face.left + (face.right-face.left)/2)* ration_w - fm.width(m_liveness)/2 ,(face.top)* ration_h - m_fontsize*filedStart + 7 - (filedStart - 1)*2), m_liveness);
                            painter->restore();
                            filedStart++;
                        }
                    }
                    else
                    {
                        painter->save();
                        painter->setPen(Qt::red);

                        if(face.uid == QString::fromLocal8Bit("LIVENESS"))
                        {
                            m_liveness = face.uid;
                            if(!m_tmrLiveness.isActive()) m_tmrLiveness.start(DISPLAY_TIMEOUT);
                            painter->drawText(QPointF((face.left + (face.right-face.left)/2)* ration_w - fm.width(m_liveness)/2 ,(face.top)* ration_h - m_fontsize*filedStart + 7 - (filedStart - 1)*2), m_liveness);
                        }
                        else
                        {
                            m_liveness.clear();
                            m_tmrLiveness.stop();
                            painter->drawText(QPointF((face.left + (face.right-face.left)/2)* ration_w - fm.width(face.uid)/2 ,(face.top)* ration_h - m_fontsize*filedStart + 7 - (filedStart - 1)*2), face.uid);
                        }

                        painter->restore();
                        filedStart++;
                    }

                    //set age
                    if(face.age.size())
                    {
                        painter->save();
                        painter->setPen(Qt::red);
                        painter->drawText(QPointF((face.left + (face.right-face.left)/2)* ration_w - fm.width(face.age)/2 ,(face.top)* ration_h - m_fontsize*filedStart + 7 - (filedStart - 1)*2), face.age);
                        painter->restore();
                        filedStart++;
                    }

                    //set gender
                    if(face.gender.size())
                    {
                        painter->save();
                        painter->setPen(Qt::red);
                        painter->drawText(QPointF((face.left + (face.right-face.left)/2)* ration_w - fm.width(face.gender)/2 ,(face.top)* ration_h - m_fontsize*filedStart + 7 - (filedStart - 1)*2), face.gender);
                        painter->restore();
                    }
                }
            }
        }
        m_faceDataList.clear();
    }
}

QImage videoWidget::getImage()
{
    QMutexLocker locker(&m_mutex);

    return this->m_img;
}

void videoWidget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    this->update();
}

videoWidget::~videoWidget()
{
    this->update();
}

void videoWidget::setCurrentResolution(int height, int width)
{
    this->m_original_h = height;
    this->m_original_w = width;
        
    if (m_original_h == 640 && m_original_w == 360)
    {
        m_ration_face_h = 0.5;
        m_ration_face_w = 0.5;        
    }
    else
    {
        if (m_original_h == 480 && m_original_w == 720)
        {
            m_ration_face_h = 2.0;            
        }
        else
        {
            m_ration_face_h = 1.0;            
        }
        m_ration_face_w = 1.0;
    }   
}

void videoWidget::faceDataSlot(QList<XFFmpeg::FaceData> list)
{
    if(!this->m_faceDataList.isEmpty()){
        m_faceDataList.clear();
    }

    this->m_faceDataList = list;
}

void videoWidget::receiveImageSlot(const QImage &image)
{
    QMutexLocker locker(&m_mutex);
    this->m_img = image;
    this->update();
}

void videoWidget::setDeviceType(DeviceType type)
{
    m_deviceType = type;
}

void videoWidget::liveScoreTimeout()
{
    m_tmrLiveScore.stop();
    m_liveScore.clear();
}

void videoWidget::livenessTimeout()
{
    m_tmrLiveness.stop();
    m_liveness.clear();
}

