#ifndef APPMANGE_H
#define APPMANGE_H

#include <QApplication>
#include <QObject>
#include <QQmlEngine>
#include<glog/logging.h>

#define LOGDIR "./LOG/"
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

    explicit AppMange(QApplication *parent=nullptr );


    void InitLOG(QString dir = LOGDIR);//初始化日志


    private:

    QApplication *app;
    QQmlEngine *qmlengine;//在Qapplication之后初始化

};

#endif // APPMANGE_H
