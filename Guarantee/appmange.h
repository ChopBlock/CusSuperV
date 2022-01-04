#ifndef APPMANGE_H
#define APPMANGE_H

#include <QApplication>
#include <QObject>
#include <QQmlEngine>
#include<glog/logging.h>
#include <QTranslator>
#include <QQmlApplicationEngine>
#include <QLocale>
#include<QDebug>
#include"client/windows/handler/exception_handler.h"
#include"folly/FBString.h"
#include"folly/File.h"
#include<QDir>
#include<QFontDatabase>
#include"ThemeManager.h"
#include<qqmlcontext.h>
#include<AppInfo.h>
#include<QQuickStyle>
#include<QStyleFactory>
/**
 *
 *1.初始化spdlog日志 main中 初始化app
 *2.声明 qmlengine
 *3.管理qml c++
 *4.日志
 *5.程序异常捕捉
 *
 **/
class AppMange :QObject{
    Q_OBJECT
public:

    explicit AppMange(int argc, char *argv[] );


    void InitLOG(const char * argv="");//初始化日志


    void exceptionHander();//google 异常捕捉

    void InitApp();
    void registerQmlTypes();
    void initializeQmlInterface();


public slots:

    static   void Appdestroy();
    static   void Appquit();
    static   void Appclose();

    static   void QMLexit();
    static   void QMLdestroy();
    static   void QMLquit();



private:

private:
    google_breakpad::ExceptionHandler *eh;
    QApplication *QApp;
    QQmlApplicationEngine *qmlengine;//在Qapplication之后初始化

};

#endif // APPMANGE_H
