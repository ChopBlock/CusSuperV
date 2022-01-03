#include "appmange.h"


AppMange::AppMange(int argc, char *argv[])


{


    QApplication  app(argc,argv) ;
    InitLOG(argv[0]);

    QApp=qApp;

    qmlengine=new QQmlApplicationEngine;

    connect(QApp,&QApplication::lastWindowClosed,this,&AppMange::Appclose);
    connect(QApp,&QApplication::aboutToQuit,this,&AppMange::Appquit);
    connect(QApp,&QApplication::destroyed,this,&AppMange::Appdestroy);


    connect(qmlengine,&QQmlApplicationEngine::exit,this,&AppMange::QMLexit);
    connect(qmlengine,&QQmlApplicationEngine::quit,this,&AppMange::QMLquit);
    connect(qmlengine,&QQmlApplicationEngine::destroyed,this,&AppMange::QMLdestroy);






    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for(auto &s:uiLanguages){
    LOG(INFO)<<s.toStdString();
    }
QString s=tr("卧室");
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CusSuperV_" + QLocale(locale).name();
 LOG(INFO)<<baseName.toStdString();
        if (translator.load("./" + baseName)) {//translator/

            app.installTranslator(&translator);
            break;
        }
    }


    const QUrl url(u"qrc:/CusSuperV/main.qml"_qs);
    QObject::connect(
                qmlengine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);


    qmlengine->load(url);
     exceptionHander();//google breakpad in process catch


    QApp->exec();
}
void YourFailureFunction() {
    // Reports something...
    LOG(ERROR)<<"YourFailureFunction";

}
void YourFailureWriter(const char* data, int size) {
    // Reports something...
    std::string src(data,size) ;


    LOG(ERROR)<<"YourFailureWriter"<<src;

}
void AppMange::InitLOG(const char * argv)
{
auto checkpath=[](const std::string path){
//std::fstream  fstrm(path.c_str(),std::ios::out) ;
//if(!fstrm.is_open()){

//    LOG(ERROR)<<path +" 打开失败！";
//}
//fstrm.close();
QDir dir(QString::fromStdString( path));
if(!dir.exists()){

    dir.mkpath(QString::fromStdString( path));
}

};

    std::string log_dir="./LOG/";

    std::string log_INFO_dir=QDir::currentPath().toStdString()+ "/LOG/INFO/";      checkpath(log_INFO_dir);
    std::string log_ERROR_dir=QDir::currentPath().toStdString()+"/LOG/ERROR/";    checkpath(log_ERROR_dir);
    std::string log_WARNNG_dir=QDir::currentPath().toStdString()+"/LOG/WARNING/"; checkpath(log_WARNNG_dir);
    std::string log_FATAL_dir=QDir::currentPath().toStdString()+"/LOG/FATAL/";    checkpath(log_FATAL_dir);


   // FLAGS_colorlogtostderr=true;

    FLAGS_stderrthreshold=google::GLOG_INFO;
    google::InstallFailureSignalHandler();
    google::InstallFailureFunction(&YourFailureFunction);
    google::InstallFailureWriter(&YourFailureWriter);

    google::InitGoogleLogging(argv);


    google::SetStderrLogging(google::GLOG_INFO);
    google::SetLogDestination(google::GLOG_INFO,log_INFO_dir.c_str());
    google::SetLogDestination(google::GLOG_ERROR,log_ERROR_dir.c_str());
    google::SetLogDestination(google::GLOG_WARNING,log_WARNNG_dir.c_str());
    google::SetLogDestination(google::GLOG_FATAL,log_FATAL_dir.c_str());







}
static bool MinidumpCallback(const wchar_t* dump_path,
                             const wchar_t* id,
                             void* context, EXCEPTION_POINTERS* exinfo,
                             MDRawAssertionInfo* assertion,
                             bool succeeded)
{
    if (succeeded)
    {
        printf("dump guid is %ws\n", id);


    }
    fflush(stdout);

    return succeeded;
}
void AppMange::exceptionHander()
{
    eh=new google_breakpad::ExceptionHandler(
                L"./dump", nullptr, MinidumpCallback, nullptr,
                google_breakpad::ExceptionHandler::HANDLER_ALL);
}





void AppMange::Appdestroy()
{
    LOG(INFO)<<"Appdestroy";
}

void AppMange::Appquit()
{
    LOG(INFO)<<"Appquit";
}

void AppMange::Appclose()
{
    LOG(INFO)<<"Appclose";
}

void AppMange::QMLexit()
{
    LOG(INFO)<<"QMLexit";
}

void AppMange::QMLdestroy()
{
    LOG(INFO)<<"QMLdestroy";
}

void AppMange::QMLquit()
{
    LOG(INFO)<<"QMLquit";
}



