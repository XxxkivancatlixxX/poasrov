#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Backend.h"
#include "camera_image_provider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Backend backend;

    QQmlApplicationEngine engine;
    
    // Register camera image provider
    engine.addImageProvider("camera", new CameraImageProvider(&backend));
    
    engine.rootContext()->setContextProperty("backend", &backend);
    engine.load(QUrl::fromLocalFile("qml/Main.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

