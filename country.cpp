#include "country.h"

Country::Country(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    connect(this, &Country::widthChanged, [this](){
        xCoef_ = width() / xDiff_;
        yCoef_ = height() / yDiff_;

        normalizedPoints_.clear();

        for (auto& p : points_) {
            normalizedPoints_.push_back({normalizePointFromCoordinates(p.p.x(), p.p.y()), p.temperature});
        }

        for (auto& i : regions_) {
            i.second->updateCoefs(xCoef_, yCoef_);
        }

        update();
    });

    connect(this, &Country::heightChanged, [this](){
        xCoef_ = width() / xDiff_;
        yCoef_ = height() / yDiff_;

        normalizedPoints_.clear();

        for (auto& p : points_) {
            normalizedPoints_.push_back({normalizePointFromCoordinates(p.p.x(), p.p.y()), p.temperature});
        }

        for (auto& i : regions_) {
            i.second->updateCoefs(xCoef_, yCoef_);
        }

        update();
    });
}

void Country::paint(QPainter *painter) {
    for (auto& [regionName, region]: regions_) {

        region->paint(painter);
    }

    painter->save();
    painter->setBrush(Qt::green);

    for (auto& i : normalizedPoints_) {
        painter->drawRect({i.p, QSize(4, 4)});
    }

    if (tempMapVisible_) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter->drawImage(0, 0, generateImage());

        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    painter->setPen(Qt::black);
    painter->setBrush(Qt::yellow);

    painter->restore();
}
