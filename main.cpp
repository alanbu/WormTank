#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include "wormtank.h"
#include "wormtankmodelview.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    WormTank wormTank;    
    WormTankModelView model(wormTank);

    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("wormTank", &wormTank);
    ctx->setContextProperty("wormTankModelView", &model);

    engine.addImageProvider(QLatin1String("wormtankframe"), new WormTankImageProvider(wormTank));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    model.startLast();
//    model.start("FoodCount", true);

    return app.exec();
}

