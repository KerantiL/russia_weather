#include "region.h"

Region::Region() {

}

Region::Region(const QVector<QPolygonF>& coordinates, QQuickItem *parent)
    : QQuickPaintedItem(parent), coordinates_(coordinates)
{

}

void Region::paint(QPainter *painter) {
    painter->save();
    painter->setPen(Qt::black);
    painter->setBrush(hovered_ ? Qt::green : Qt::red);
    painter->setRenderHint(QPainter::Antialiasing, true);

    auto coordinates = coordinates_;
    for (auto& i : coordinates) {
        for (auto& j : i) {
            j.rx() *= xCoef_;
            j.ry() *= yCoef_;
        }
    }

    for (auto& coordinatesPart : coordinates) {
        painter->drawPolygon(coordinatesPart);
    }


    painter->restore();
}
