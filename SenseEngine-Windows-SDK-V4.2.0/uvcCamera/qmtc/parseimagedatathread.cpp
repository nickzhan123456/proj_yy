#include "parseimagedatathread.h"
#include<QFile>
#include<QDebug>
#include<QTime>
QMutex* parseImageDataThread::s_imutex = new QMutex;
parseImageDataThread::parseImageDataThread(QObject *parent) : QObject(parent)
{

}
void parseImageDataThread::doParseImageData(unsigned long long tamps, QByteArray data)
{
//    qDebug() << "[parseImageData]data: " << data << ", size: " << data.size();
//    QMutexLocker locker(s_imutex);
    //qDebug()<<"doParseImageData()";
    if (data.size() > 0)
    {
        QString timeNumber = QString::number(QDateTime::currentMSecsSinceEpoch() / 1000);
        int trackId  = data.at(0) & 0x000000FF;
        trackId |= ((data.at(1) <<8)&0x0000FF00);
        trackId |= ((data.at(2) <<16)&0x00FF0000);
        trackId  |= ((data.at(3)<<24)&0xFF000000);
        int reoIdLength = data.at(4) & 0x000000FF;
        reoIdLength |= ((data.at(5) <<8)&0x0000FF00);
        reoIdLength |= ((data.at(6) <<16)&0x00FF0000);
        reoIdLength  |= ((data.at(7)<<24)&0xFF000000);

        QByteArray reoData =  data.mid(8, reoIdLength);
        if(reoData.isEmpty())
        {
            reoData = "null";
        }
//        QString reoData =  data.mid(8, reoIdLength);

        //RGB人脸切图
        int rgbStartIndex = 8+reoIdLength;
        int rgbLen = data.at(rgbStartIndex) & 0x000000FF;
        rgbLen |= ((data.at(rgbStartIndex + 1) <<8)&0x0000FF00);
        rgbLen |= ((data.at(rgbStartIndex + 2) <<16)&0x00FF0000);
        rgbLen  |= ((data.at(rgbStartIndex + 3)<<24)&0xFF000000);
        //qDebug() << "rgb face rgblen " << rgbLen;
        if(rgbLen > 0)
        {
            QByteArray rgbData = data.mid(rgbStartIndex + 4, rgbLen);
            QFile file(QString("image/%1_%2_%3").arg(reoData.data()).arg(timeNumber).arg("imageFaceRGB.jpg"));
            file.open(QIODevice::WriteOnly);
            file.write(rgbData);
            file.flush();
            file.close();
        }

        //rgb 人脸背景图
        int rgbBackStartIndex = 8 + reoIdLength + 4 + rgbLen;
        int rgbBackLen =  data.at(rgbBackStartIndex) & 0x000000FF;
        rgbBackLen |= ((data.at(rgbBackStartIndex + 1) <<8)&0x0000FF00);
        rgbBackLen |= ((data.at(rgbBackStartIndex + 2) <<16)&0x00FF0000);
        rgbBackLen  |= ((data.at(rgbBackStartIndex + 3)<<24)&0xFF000000);
        //qDebug() << "rgbBack face rgbBackLen " << rgbBackLen;
        if(rgbBackLen > 0)
        {
            QByteArray rgbBackData = data.mid(rgbBackStartIndex + 4, rgbBackLen);
            QFile file2(QString("image/%1_%2_%3").arg(reoData.data()).arg(timeNumber).arg("imageFaceBackRGB.jpg"));//实际上目前设备上报该图是与imageFaceRGB一样的
            file2.open(QIODevice::WriteOnly);
            file2.write(rgbBackData);
            file2.flush();
            file2.close();
        }
//        QByteArray rgbBackData = data.mid(16+reoIdLength+ rgbLen, rgbBackLen);
//        qDebug()<< "[parseImageData]trackId: " << trackId << ", ReoIDLength:" << reoIdLength
//                 << ", rgbLen: " << rgbLen << ", rgbBackLen:" <<rgbBackLen
//                 << ", total: " << 8+reoIdLength+8+rgbLen+rgbBackLen;
//        if (reoIdLength+rgbLen+ rgbBackLen <0) {
//            return;
//        }
        //IR背景图
        int irBackStartIndex = 8 + reoIdLength + 4 + rgbLen + 4 + rgbBackLen;
        if(data.length() < irBackStartIndex)
        {
            //M10不含ir图片
            //qDebug() << "[doParseImageData] " << "   no ir image";
            return;
        }

        int irLen =  data.at(irBackStartIndex) & 0x000000FF;
        irLen |= ((data.at(irBackStartIndex + 1) <<8)&0x0000FF00);
        irLen |= ((data.at(irBackStartIndex + 2) <<16)&0x00FF0000);
        irLen  |= ((data.at(irBackStartIndex + 3)<<24)&0xFF000000);
        //qDebug() << "irBack face irBackLen " << irLen;
        if (irLen > 0)
        {
            QByteArray irData = data.mid(irBackStartIndex + 4, irLen);
            //            QFile file(QString("image/%1_%2_%3").arg(reoData.data()).arg(tamps).arg("imageFaceBackIR.jpg"));
            QFile file(QString("image/%1_%2_%3").arg(reoData.data()).arg(timeNumber).arg("imageFaceBackIR.jpg"));
            file.open(QIODevice::WriteOnly);
            file.write(irData);
            file.flush();
            file.close();
            //qDebug()<<"save IR image!";
        }
    }
}
