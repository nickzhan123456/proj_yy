#pragma once
#include <QThread>
#include <QTime>
class XVideoThread:public QThread
{
	Q_OBJECT
public:
    static XVideoThread *Get();
    void Close();

	void run();
	XVideoThread();
    virtual ~XVideoThread();    

private:    
    bool isexit          = false;
};

