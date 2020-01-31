#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QDebug>
#include "match3model.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Match3Model>("custom.Match3Model", 1, 0, "Match3Model");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    // signals

    QObject *rootObject = engine.rootObjects().first();
    QObject *matchModel = rootObject->findChild<QObject*>("matchModel");
    QObject *gameBoard= rootObject->findChild<QObject*>("gameBoard");

    QObject::connect(gameBoard, SIGNAL(elementAdded(int)),
                     matchModel, SLOT(OnElementAdd(int)));
    //    qDebug() <<


    //    QQmlEngine boardEngine;

    //    QQmlComponent component(&boardEngine, "GameBoard.qml");
    //    QObject *object = boardEngine.get

    return app.exec();
}
