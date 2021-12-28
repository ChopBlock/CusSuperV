#include "spdlog/spdlog.h"
#include <QApplication>

#include <QLocale>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QTranslator>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv); // QApplication<-QGuiAppaction<-QCoreAPPaction

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CusSuperV_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/CusSuperV/main.qml"_qs);
    QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated, &app,
    [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
