#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include "core/AppEngine.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set application info
    app.setApplicationName("ModbusBMS");
    app.setOrganizationName("SmartHome");
    app.setApplicationVersion("1.0.0");
    
    // Create app engine
    AppEngine appEngine;
    appEngine.initialize();
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose C++ objects to QML
    engine.rootContext()->setContextProperty("deviceManager", appEngine.deviceManager());
    engine.rootContext()->setContextProperty("appLogger", appEngine.logger());
    engine.rootContext()->setContextProperty("appSettings", appEngine.settings());
    engine.rootContext()->setContextProperty("appEngine", &appEngine);
    
    // Load QML
    const QUrl url(QStringLiteral("qrc:/ModbusBMS/Main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    // Start the application
    appEngine.start();
    
    return app.exec();
}
