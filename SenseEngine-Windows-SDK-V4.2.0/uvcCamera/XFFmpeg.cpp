#include "XFFmpeg.h"
#include <QDebug>
#include <QString>
#include <QTime>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QThread>
#include <QImage>

#include "ai_camera.h"

QMutex XFFmpeg::s_mutex;
XFFmpeg *XFFmpeg::m_pInstance = nullptr;

XFFmpeg *XFFmpeg::Get()
{
    if(!m_pInstance)//first recognition
    {
        QMutexLocker locker(&s_mutex);//Add mutex.

        if(!m_pInstance)//second recognition
            m_pInstance = new XFFmpeg();
    }
    return m_pInstance;
}

void XFFmpeg::Setup(void* dev)
{
    //qDebug()<<"dev of XFFmpeg set to "<<dev;
    //qDebug() << "-----------void XFFmpeg::Setup thread is " << QThread::currentThread();
    this->dev = dev;
}

XFFmpeg::XFFmpeg()
{
    errorbuf[0] = '\0';
    qRegisterMetaType<QList<XFFmpeg::FaceData>>("QList<FaceData>");
}


XFFmpeg::~XFFmpeg()
{
}

int XFFmpeg::Open(QString deviceName)
{    
    Close();
    cameraName = deviceName;
    int result = ConnectCamera(dev, deviceName.toUtf8().data());
    int width = 0,height = 0;
    GetResolution(dev,&width,&height);
    emit currentResolution(height,width);

    return result;
}

QString XFFmpeg::getDeviceName()
{
    return this->cameraName;
}
void XFFmpeg::Close()
{
    DisconnectCamera(dev);
}

int XFFmpeg::parseFrame()
{
 //   QMutexLocker locker(&mutex);

    int width = 0,height = 0;
    GetResolution(dev,&width,&height);

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    char tempData[1024] = {0};
    uchar* imageData = image.bits();
    int result = GetFrame(dev, reinterpret_cast<char*>(imageData) ,width*height*4,tempData,1024);
    //qDebug() <<"parseFrame GetFrame Result : "<< result;
    if(m_isMirrored)
    {
        image = image.mirrored(true, false);//Add a mirror
        if(width == 720 || width == 360)
        {
            MirroredWidth = 720;
        }
        else if(width == 1280 || width == 640)
        {
            MirroredWidth = 1280;
        }
    }

    if (result != 0)
    {
        return result;
    }
    emit receiveImage(image);

    if(strlen(tempData)>0 && m_isPaintFaceRect)
    {
        if(m_isFrameReport)
        {
            //qDebug() << "frameReport : " << tempData<<endl;
            parseVerifyData(tempData);
        }
    }
    if(!m_jsonData.isEmpty())
    {
//        qDebug()<<m_jsonData.data()<<endl;
        parseVerifyData(m_jsonData.data());
        m_parseVerifyDataCount++;
        if(m_parseVerifyDataCount %6 == 0)
        {
            if(m_jsonDataCount == m_jsonDataCountNow)
            {
                m_jsonData.clear();
            }
            m_jsonDataCountNow = m_jsonDataCount;

        }
    }
    return 0;
}

void XFFmpeg::parseVerifyData(char* tempData)
{
//    static bool isMask = false;
//    static int isMaskNum = 0;
//    isMaskNum++;
//    if((isMaskNum %= 100) == 99)
//    {
//        isMask = false;
//    }
    if(!m_isPaintFaceRect)// Prevent auto reporting, switching IR off drawing face frame
    {
        return;
    }
    QString verity_data(tempData);
    if (verity_data.isEmpty()) {
        //qDebug() << "[XFFmpeg::parseVerifyData] verity_data is empty...";
        return;
    }

    int index = verity_data.lastIndexOf('}');
    //qDebug() << QDateTime::currentDateTime().toString("yyyy_MM_dd hh:mm:ss") << "verity data is : " << verity_data.mid(0,index+1).toUtf8();
    QList<FaceData> m_faceRectList;
    // 1. create QJsonParseError object，Used to obtain parsing results
    QJsonParseError error;
    // 2. Get using static functions QJsonDocument object
    QJsonDocument jsonDocument = QJsonDocument::fromJson(verity_data.mid(0,index+1).toUtf8(), &error);
    // 3. Process according to the result of parsing
    if (error.error == QJsonParseError::NoError) {
        if (!(jsonDocument.isNull() || jsonDocument.isEmpty()))
        {
            if (jsonDocument.isObject())
            {
                QVariantMap result = jsonDocument.toVariant().toMap();
                foreach (QVariant plugin, result["track_info"].toList())
                {
                    QVariantMap faceMap = plugin.toMap();
                    QVariantMap faceRectMap = faceMap["face_info"].toMap();                    
                    QVariantMap rectMap = faceRectMap["rect"].toMap();


                    FaceData faceData;
                    faceData.status = faceMap["recognition_status"].toUInt();
                    int mask_sign = 0;
                    mask_sign = faceMap["mask_sign"].toInt();
                    int safety_hat = 0;
                    safety_hat = faceMap["safety_hat"].toInt();

//                    if(faceMap.contains("recognize_score") || faceMap.contains("verify_score") || faceMap.contains("liveness_score"))
//                    {
//                        if(mask_sign == 1 || mask_sign == 2)
//                        {
//                            faceData.mask = QString::fromLocal8Bit("MASKING");
//                            isMask = true;
//                        }
//                        else
//                        {
//                            isMask = false;
//                        }
//                    }

                    if(faceMap.contains("attr_info"))
                    {
                        QVariantMap attrMap = faceMap["attr_info"].toMap();
                        int gender = attrMap["gender"].toInt();
                        int age    = attrMap["age"].toInt();
                        if(gender == 0)
                        {
                            faceData.gender = "gender : female"; //woman
                        }
                        else if(gender == 1)
                        {
                            faceData.gender = "gender : male"; //man
                        }

                        if(age >= 0)
                        {
                            faceData.age    = "age : "    + QString::number(attrMap["age"].toInt());
                        }
                    }

                    if(faceMap.contains("liveness_score"))
                    {                                                
                        double liveness_score = faceMap["liveness_score"].toDouble(nullptr);
                        if(liveness_score > 0)
                        {
                            faceData.livescore = QString("liveness_score:").append(QString::number(liveness_score,'f'));
                        }
                    }

                    /*if(isMask == true)
                    {
                        faceData.mask = QString::fromLocal8Bit("MASKING");
                    }
                    else */if(mask_sign == 1 || mask_sign == 2)
                    {
                        faceData.mask = QString::fromLocal8Bit("MASKING");
                    }
                    /*else */if(safety_hat == 1 || safety_hat == 2)
                    {
                        faceData.hat = QString::fromLocal8Bit("HATTING");
                    }

                    switch (faceData.status)
                    {
                        case 0:
                        {
                            faceData.uid = QString::fromLocal8Bit("UNKNOW");
                            break;
                        }
                        case 1:
                        {
                            faceData.uid = QString::fromLocal8Bit("NOLIVENESS");
                            break;
                        }
                        case 2:
                        {
                            faceData.uid = faceMap["recognition_id"].toString();
                            break;
                        }
                        case 3:
                        {
                            faceData.uid = QString::fromLocal8Bit("STRANGER");
                            break;
                        }
                        case 4:
                        {
                            faceData.uid = faceMap["recognition_id"].toString();
                            break;
                        }
                        case 5:
                        {
                            faceData.uid = QString::fromLocal8Bit("VERIFY_PASS");
                            break;
                        }
                        case 6:
                        {
                            //faceData.uid = QString::fromLocal8Bit("VERIFY_FAIL");
                            break;
                        }
                        default:
                            break;
                    }

                    if(m_isMirrored)
                    {
                        faceData.top = rectMap["top"].toInt();
                        faceData.right = MirroredWidth - rectMap["right"].toInt();
                        faceData.left = MirroredWidth - rectMap["left"].toInt();
                        faceData.bottom = rectMap["bottom"].toInt();
                    }
                    else
                    {
                        faceData.top = rectMap["top"].toInt();
                        faceData.right = rectMap["right"].toInt();
                        faceData.left = rectMap["left"].toInt();
                        faceData.bottom = rectMap["bottom"].toInt();
                    }

                    if(faceData.status != 6)//not display VERIFY_FAIL
                    {
                        m_faceRectList.append(faceData);
                    }
                }
                emit faceDataSignal(m_faceRectList);
            } else if (jsonDocument.isArray()) {
                // ...
            }
        }
    } else {
        // Checking for error types
        //qDebug() << "Face verity json error";
        //qDebug() << "Error verity data is : " << verity_data.mid(0,index+1).trimmed();
    }
}

void XFFmpeg::setFaceRectEnable(bool enable)
{
    this->m_isPaintFaceRect = enable;
}

void XFFmpeg::setIsMirrored(bool isMirrored)
{
    this->m_isMirrored = isMirrored;
}


void XFFmpeg::setIsFrameReport(bool isFrameReport)
{
    this->m_isFrameReport = isFrameReport;
}

void XFFmpeg::setJsonData(QByteArray jsonData)
{
    this->m_jsonData = jsonData;
    this->m_jsonDataCount++;
    this->m_jsonDataCount %= 100;
}
