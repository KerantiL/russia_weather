#ifndef TEMPARATURE_H
#define TEMPARATURE_H

#include <cmath>
#include <QPoint>
#include <QVector>

namespace temperature {

struct TemperaturePoint {
    QPointF p;
    double temperature;
};

double dist(QPointF p1, QPointF p2);

double generateTemperature(QPointF point, const QVector<TemperaturePoint>& points);

}

#endif // TEMPARATURE_H
