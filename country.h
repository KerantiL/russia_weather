#ifndef COUNTRY_H
#define COUNTRY_H

#include <QQuickPaintedItem>
#include <map>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>
#include <limits>
#include <fstream>
#include "database.h"

#include "region.h"
#include "Temparature.h"

class Country : public QQuickPaintedItem
{
    Q_OBJECT
public:
    Country(QQuickItem *parent = nullptr);

    void paint(QPainter* painter) override;

    Q_INVOKABLE bool load(const QString& path) {
        auto [minX, maxX, minY, maxY] = minmaxXY(path);
        xMin_ = minX; xMax_ = maxX; yMin_ = minY; yMax_ = maxY;
        xDiff_ = xMax_ - xMin_;
        yDiff_ = yMax_ - yMin_;
        xCoef_ = width() / xDiff_;
        yCoef_ = height() / yDiff_;

        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) { return false; }

        auto json = QJsonDocument::fromJson(file.readAll()).object();

        for (auto it = json.constBegin(); it < json.constEnd(); it++) {
            QVector<QPolygonF> coords;
            auto itCont = it.value().toObject();

            for (auto it2 = itCont.constBegin(); it2 < itCont.constEnd(); it2++) {
                QPolygonF polygon;
                auto it2Cont = it2.value().toArray();

                for (auto it3 = it2Cont.begin(); it3 < it2Cont.end(); it3++) {
                    auto it3Cont = it3->toArray();

                    auto point = xyFromCoordinates(it3Cont[0].toDouble(), it3Cont[1].toDouble());
                    point.rx() -= xMin_;
                    point.ry() -= yMin_;

                    polygon.push_back(point);
                }

                coords.push_back(polygon);
            }

            regions_[it.key()] = new Region(coords);
        }

        Database db;

        auto res = db.cities();

        for (auto& i : res) {
            points_.push_back({{i.latitude, i.longitude}, db.getTemperatureFromCityId(i.id) + 50});
        }


        update();

        file.close();
        return true;
    }

    Q_INVOKABLE QPointF xyFromCoordinates(double latitude, double longitude) {
        return {longitude < 0 ? 360 + longitude : longitude, 90 - latitude};
    }

    Q_INVOKABLE QPointF normalizePoint(QPointF point) {
        point.rx() = (point.x() - xMin_) * xCoef_;
        point.ry() = (point.y() - yMin_) * yCoef_;
        return point;
    }

    Q_INVOKABLE QPointF normalizePointFromCoordinates(double latitude, double longitude) {
        return normalizePoint(xyFromCoordinates(latitude, longitude));
    }

    Q_INVOKABLE QPointF coordinatesFromXy(double x, double y) {
        return {90 - y, x > 180 ? x - 360 : x};
    }

    Q_INVOKABLE QPointF coordinatesFromXyOnMap(double x, double y) {
        return coordinatesFromXy((x / xCoef_) + xMin_, (y / yCoef_) + yMin_);
    }

    Q_INVOKABLE void getRegion(QPointF point) {
        for (auto& [key, value] : regions_) {
            if (value->containsPoint(point)) {
                currentRegion_ = value;
                currentRegionName_ = key;
                value->setHovered(true);
                update();
            } else {
                value->setHovered(false);
                update();
            }
        }
    }

    Q_INVOKABLE void setVisibleTempMap(bool value) {
        if (tempMapVisible_ != value) {
            tempMapVisible_ = true;
            update();
        }
    }

    Q_INVOKABLE QVariantMap getCurrentRegionInfo() {
        return {{"name", currentRegionName_}};
    }

private:

    QImage generateImage() {
        QImage image(width(), height(), QImage::Format_ARGB32);

        for (int i = 0; i < width(); i++) {
            for (int j = 0; j < height(); j++) {
                temperature::TemperaturePoint p{{static_cast<double>(i), static_cast<double>(j)}, 0};
                p.temperature = temperature::generateTemperature(p.p, normalizedPoints_);
                image.setPixelColor(i, j, QColor((255 * p.temperature / 100), 0, 255 - (255 * p.temperature / 100)));
            }
        }

        return image;
    }

    std::tuple<double, double, double, double> minmaxXY(const QString& path) {
        QFile file(path);
        double minY = std::numeric_limits<double>::max();
        double maxY = std::numeric_limits<double>::min();
        double minX = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::min();

        if (!file.open(QIODevice::ReadOnly)) { return {0, 0, 0, 0}; }

        auto json = QJsonDocument::fromJson(file.readAll()).object();

        for (auto it = json.constBegin(); it < json.constEnd(); it++) {
            auto itCont = it.value().toObject();

            for (auto it2 = itCont.constBegin(); it2 < itCont.constEnd(); it2++) {
                auto it2Cont = it2.value().toArray();

                for (auto it3 = it2Cont.begin(); it3 < it2Cont.end(); it3++) {
                    auto it3Cont = it3->toArray();

                    auto point = xyFromCoordinates(it3Cont[0].toDouble(), it3Cont[1].toDouble());

                    if (minX > point.x()) {
                        minX = point.x();
                    }

                    if (maxX < point.x()) {
                        maxX = point.x();
                    }

                    if (minY > point.y()) {
                        minY = point.y();
                    }

                    if (maxY < point.y()) {
                        maxY = point.y();
                    }
                }
            }
        }

        file.close();
        return {minX, maxX, minY, maxY};
    }

    bool tempMapVisible_ = false;
    QString currentRegionName_;
    Region* currentRegion_;
    std::map<QString, Region*> regions_;
    QVector<temperature::TemperaturePoint> points_;
    QVector<temperature::TemperaturePoint> normalizedPoints_;
    double xCoef_ = 1;
    double yCoef_ = 1;
    double xMin_;
    double xMax_;
    double yMin_;
    double yMax_;
    double xDiff_;
    double yDiff_;
};

#endif // COUNTRY_H
