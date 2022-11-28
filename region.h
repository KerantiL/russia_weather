#ifndef REGION_H
#define REGION_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QVector>
#include <QList>
#include <QPolygonF>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Region : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit Region();

    explicit Region(const QVector<QPolygonF>& coordinates, QQuickItem *parent = nullptr);

    void paint(QPainter* painter) override;

    void updateCoefs(double xCoef, double yCoef) {
        xCoef_ = xCoef;
        yCoef_ = yCoef;
    }

    bool containsPoint(QPointF p) const {
        p.rx() /= xCoef_;
        p.ry() /= yCoef_;

        for (auto& i : coordinates_) {
            if (i.containsPoint(p, Qt::FillRule::WindingFill)) {
                return true;
            }
        }

        return false;
    }

    void setHovered(bool hovered) {
        if (hovered != hovered_) {
            hovered_ = hovered;
        }
    }

signals:

private:
    QVector<QPolygonF> coordinates_;
    double xCoef_ = 1;
    double yCoef_ = 1;
    bool hovered_ = false;
    QColor color_ = Qt::red;
};

#endif // REGION_H
