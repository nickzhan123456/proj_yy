#include "XVideoThread.h"
#include "XFFmpeg.h"
#include <QDebug>

XVideoThread::XVideoThread()
{

}

XVideoThread::~XVideoThread()
{        

}

XVideoThread* XVideoThread::Get()
{
    static XVideoThread vt;
    return &vt;
}

void XVideoThread::Close()
{
    isexit = true;
    //qDebug() << "XVideoThread::~XVideoThread thread exit | ThreadId is " << QThread::currentThreadId();
    quit();
    wait();
}

void XVideoThread::run()
{
    //qDebug() << "XVideoThread::run thread start | ThreadId is " << QThread::currentThreadId();
    isexit = false;
    while (!isexit)
    {
        if(XFFmpeg::Get()->parseFrame() != 0)
        {
            continue;
        }
        msleep(33);
    }
}
