#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "advancedswipeview.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<AdvancedSwipeView>("Custom.Controls", 1, 0, "AdvancedSwipeView");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
