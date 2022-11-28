#include "Temparature.h"
#include <QDebug>

namespace temperature {

double dist(QPointF p1, QPointF p2) {
    if (p1 == p2) {
        return 0;
    }

    return 1 / std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
}

double generateTemperature(QPointF point, const QVector<TemperaturePoint>& points) {
    double top = 0;
    double bottom = 0;

    for (auto& other_point : points) {
        double dist_ = dist(point, other_point.p);
        if (dist_ == 0) {
            return other_point.temperature;
        }

        top += dist_ * other_point.temperature;
        bottom += dist_;
    }

    return top / bottom;
}

}
