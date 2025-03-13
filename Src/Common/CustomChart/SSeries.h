#ifndef SSERIES_H
#define SSERIES_H

#include <QPen>
#include <QPolygonF>
#include <QQuickItem>

#include "../CommonDef.hpp"

namespace SChart {
Q_NAMESPACE
enum class SSeriesType
{
    SeriesStart = 0,
    LineSeries,
    ScatterSeries,
    SeriesEnd,
};
Q_ENUM_NS(SSeriesType);


class SSeries : public QQuickItem
{
    Q_OBJECT
    QML_DECL_PROPERTY(QString, color, "black")
    QML_DECL_PROPERTY(int, maxPointCount, 2000)

public:
    SSeriesType seriesType;
    const QMultiMap<double, double>& pointDatas()
    {
        return m_data;
    }

protected:
    SSeries(QQuickItem* parent = nullptr);

public slots:
    virtual void appendPoints(QVector<QPointF> point);
    virtual void appendPoint(QPointF point);

    virtual void appendPoint(double x, double y, bool bForward = false);

    virtual void appendPoints(QVector<QPoint> point);
    virtual void appendPoint(QPoint point);

    virtual void forwardAppendPoint(QPointF point);

    virtual void setPoints(QVector<QPointF> points); // 设置series数据
    virtual void setPoints(QVector<QPoint> points); // 设置series数据

    virtual QPointF at(int index);
    virtual int count();

    virtual void clean(){setPoints(QVector<QPointF>{});} // 清除series数据

protected:
    QMultiMap<double, double> m_data;
};


class SLineSeries : public SSeries
{
    Q_OBJECT
    QML_DECL_PROPERTY(Qt::PenStyle, lineStyle, Qt::SolidLine)
    QML_DECL_PROPERTY(int, penWidth, 0)

public:
    SLineSeries(QQuickItem* parent = nullptr);

signals:

};


class SScatterSeries : public SSeries
{
    Q_OBJECT
    QML_DECL_PROPERTY(int, radius, 5)

public:
    SScatterSeries(QQuickItem* parent = nullptr);

signals:

};

}


#endif // SSERIES_H
