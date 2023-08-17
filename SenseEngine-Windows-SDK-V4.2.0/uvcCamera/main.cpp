#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDateTime>
#include <QMutex>
#include <QTextStream>
#include <QDebug>
#ifdef WindowsBuild
//#include <tchar.h>
//#include <Windows.h>
//#include <DbgHelp.h>

//#pragma comment(lib, "user32.lib")
//int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
//{
//    // Defining function Pointers
//    typedef BOOL(WINAPI * MiniDumpWriteDumpT)(
//                HANDLE,
//                DWORD,
//                HANDLE,
//                MINIDUMP_TYPE,
//                PMINIDUMP_EXCEPTION_INFORMATION,
//                PMINIDUMP_USER_STREAM_INFORMATION,
//                PMINIDUMP_CALLBACK_INFORMATION
//                );
//    // Get the "MiniDumpWriteDump" function from the "dbghelp.dll" library
//    MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
//    HMODULE hDbgHelp = LoadLibrary(_T("DbgHelp.dll"));
//    if (NULL == hDbgHelp)
//    {
//        return EXCEPTION_CONTINUE_EXECUTION;
//    }
//    pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

//    if (NULL == pfnMiniDumpWriteDump)
//    {
//        FreeLibrary(hDbgHelp);
//        return EXCEPTION_CONTINUE_EXECUTION;
//    }
//    // Create a DMP file
//    TCHAR szFileName[MAX_PATH] = { 0 };
//    TCHAR szVersion[] = L"DumpFile";
//    SYSTEMTIME stLocalTime;
//    GetLocalTime(&stLocalTime);
//    wsprintf(szFileName, L"%s-%04d%02d%02d-%02d%02d%02d.dmp",
//             szVersion, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
//             stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
//    HANDLE hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
//                                  FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
//    if (INVALID_HANDLE_VALUE == hDumpFile)
//    {
//        FreeLibrary(hDbgHelp);
//        return EXCEPTION_CONTINUE_EXECUTION;
//    }
//    // Write to DMP file
//    MINIDUMP_EXCEPTION_INFORMATION expParam;
//    expParam.ThreadId = GetCurrentThreadId();
//    expParam.ExceptionPointers = pExceptionPointers;
//    expParam.ClientPointers = FALSE;
//    pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
//                         hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &expParam : NULL), NULL, NULL);
//    // Release the file
//    CloseHandle(hDumpFile);
//    FreeLibrary(hDbgHelp);
//    return EXCEPTION_EXECUTE_HANDLER;
//}

//LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
//{
//    // Exception filtering or notification
//    if (IsDebuggerPresent()) {
//        return EXCEPTION_CONTINUE_SEARCH;
//    }
//    return GenerateMiniDump(lpExceptionInfo);
//}
#endif

static QFile file;
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //set lock
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toUtf8();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }
    QTextStream stream(&file);
    stream << "[ " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz") << "] "<< localMsg << "\r\n";
    file.flush();

    // unlock
    mutex.unlock();
}

void logMessageOutputQt5(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static qint64 max = 100*1024*1024;//100 M Log segmentation;
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("Debug:");
        break;
    case QtWarningMsg:
        text = QString("Warning:");
        break;
    case QtCriticalMsg:
        text = QString("Critical:");
        break;
    case QtFatalMsg:
        text = QString("Fatal:");
        abort();
    default:
        break;
    }
    QString message = QString("[%1] %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")).arg(msg);
    QString filePath =  QGuiApplication::applicationDirPath() + "/current_log.txt";
    file.setFileName(filePath);
    if (file.size() > max) { //Intercepts files when the specified size is exceeded
        QString newFileName =  QString(QGuiApplication::applicationDirPath() + "/%1_log.txt").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"));
        file.rename(newFileName);
        QFile logfile(filePath);
        file.open(QIODevice::ReadWrite);
        //        logfile.open(QIODevice::ReadWrite);
        qint64 nresult = 1024; //1024; // Link the last 1KB data of the previous file to the next file
        file.seek(file.size() - nresult);
        char lbuffer[256];
        int count;
        while ((count = file.read(lbuffer, 256)) == 256) {
            logfile.write(lbuffer, count);
        }
        logfile.write(message.toLatin1(), message.count());
        file.close();
        //        logfile.close();
    } else {
        QTextStream text_stream(&file);
        //        file.open(QIODevice::ReadWrite | QIODevice::Append);
        text_stream << message << endl;
        file.flush();
        //        file.close();
    }
    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = a.arguments();
    if(argc > 1)
    {
        if(args[1] == "-d")
        {
            QString filePath = QString(QGuiApplication::applicationDirPath() + "/%1_log.txt").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"));
            file.setFileName(filePath);
            if(!QFile::exists(filePath))
            {
                file.open(QIODevice::ReadWrite);
                file.close();
            }
            //qDebug() << "file creat....";
            if(!file.open(QIODevice::ReadWrite))
            {
                //qDebug() << "--- File open error! ---" << endl;
            }
            //Registered MessageHandler
            qInstallMessageHandler(myMessageOutput); //Registered MessageHandler
            //            qInstallMessageHandler(logMessageOutputQt5); //Registered MessageHandler
        }
    }

#ifdef _DEBUG
    //QString filePath = QString(QApplication::applicationDirPath() + "/%1_log.txt").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"));
    //file.setFileName(filePath);
    //if(!QFile::exists(filePath))
    //{
    //    file.open(QIODevice::ReadWrite);
    //    file.close();
    //}
    //if(!file.open(QIODevice::ReadWrite))
    //{
    //    qDebug() << "--- File open error! ---" << endl;
    //}
    //registered messagehandler
    //    qInstallMessageHandler(myMessageOutput);
#endif

    //    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//Register the exception capture function
#ifdef WindowsBuild
//    SetUnhandledExceptionFilter(ExceptionFilter);
#endif
//    setvbuf(stdout,NULL,_IONBF,0);

    MainWindow w;
    w.show();
    int ret = a.exec();
#ifdef _DEBUG
    file.close();
#endif
    return ret;
}
