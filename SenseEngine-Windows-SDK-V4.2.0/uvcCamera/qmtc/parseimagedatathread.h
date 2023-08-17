#ifndef PARSEIMAGEDATATHREAD_H
#define PARSEIMAGEDATATHREAD_H
#include<QObject>
#include<QMutex>
class parseImageDataThread : public QObject
{
    Q_OBJECT
public:
    explicit parseImageDataThread(QObject *parent = nullptr);
public slots:
    void doParseImageData(unsigned long long tamps, QByteArray data);
public:
       static QMutex* s_imutex;
};

#endif // PARSEIMAGEDATATHREAD_H
