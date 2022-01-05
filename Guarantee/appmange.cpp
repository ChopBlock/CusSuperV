#include "appmange.h"


AppMange::AppMange(int argc, char *argv[])


{


    QApplication  app(argc,argv) ;


    QApp=qApp;

    qmlengine=new QQmlApplicationEngine;

    //  init app
    InitLOG(argv[0]);//
    exceptionHander();//google breakpad in process catch
    InitApp();//
    registerQmlTypes();
    initializeQmlInterface();



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

    for (const QString &locale : uiLanguages) {
        const QString baseName = "CusSuperV_" + QLocale(locale).name();

        if (translator.load("./" + baseName)) {//translator/

            app.installTranslator(&translator);
            break;
        }
    }


    const QUrl url(u"qrc:/main.qml"_qs);//u"qrc:/CusSuperV/main.qml"_qs
    QObject::connect(
                qmlengine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);


    qmlengine->load(url);



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


     FLAGS_colorlogtostderr=true;

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

void AppMange::InitApp()
{    // Init. application

    QApp->setApplicationName(APP_NAME);
    QApp->setApplicationVersion(APP_VERSION);
    QApp->setOrganizationName(APP_DEVELOPER);
    QApp->setOrganizationDomain(APP_SUPPORT_URL);

    // Set application style
    QApp->setStyle(QStyleFactory::create("Fusion"));
    QQuickStyle::setStyle("Fusion");
    // Load Roboto fonts from resources
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Bold.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Regular.ttf");

    QFontDatabase::addApplicationFont(":/assets/fonts/Hack-Bold.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Hack-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Hack-Italic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Hack-Regular.ttf");

    // Set Roboto as default app font
    QFont font("Hack");
#if defined(Q_OS_WIN)
    font.setPointSize(9);
#elif defined(Q_OS_MAC)
    font.setPointSize(13);
#elif defined(Q_OS_LINUX)
    font.setPointSize(10);
#endif
    qApp->setFont(font);


}

void AppMange::registerQmlTypes()
{

}

void AppMange::initializeQmlInterface()
{// Initialize modules
    auto miscThemeManager = &Misc::ThemeManager::instance();
    // Operating system flags
    bool isWin = false;
    bool isMac = false;
    bool isNix = false;
#if defined(Q_OS_MAC)
    isMac = true;
#elif defined(Q_OS_WIN)
    isWin = true;
#else
    isNix = true;
#endif

    // Qt version QML flag
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const bool qt6 = false;
#else
    const bool qt6 = true;
#endif

    // Register C++ modules with QML
    QQmlContext *c = qmlengine->rootContext();
    c->setContextProperty("Cpp_Qt6", qt6);
    c->setContextProperty("Cpp_IsWin", isWin);
    c->setContextProperty("Cpp_IsMac", isMac);
    c->setContextProperty("Cpp_IsNix", isNix);
    c->setContextProperty("Cpp_ThemeManager", miscThemeManager);

    // Register app info with QML
    c->setContextProperty("Cpp_AppName", qApp->applicationName());
    c->setContextProperty("Cpp_AppUpdaterUrl", APP_UPDATER_URL);
    c->setContextProperty("Cpp_AppVersion", qApp->applicationVersion());
    c->setContextProperty("Cpp_AppOrganization", qApp->organizationName());
    c->setContextProperty("Cpp_AppOrganizationDomain", qApp->organizationDomain());
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



