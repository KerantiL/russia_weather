#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QString>
#include <QPolygonF>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "database.h"
#include <chrono>

#include <QVector>
#include <map>

#include "region.h"

class Core : public QObject
{
    Q_OBJECT
public:
    Core(QObject* parent = nullptr);
    void init();

signals:
    void searched(double temp);

public slots:
    void updateTemp();
    void updateTemp2();
    void search(double latitude, double longitude);

private:
    Database db_;
    QNetworkAccessManager nam_;
    int counter_;
    QVector<double> temps_;
};

#endif // CORE_H
