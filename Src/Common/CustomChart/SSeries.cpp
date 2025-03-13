#include "SSeries.h"

using namespace SChart;

// base
SSeries::SSeries(QQuickItem* parent)
    : QQuickItem{parent}
{
}

void SSeries::appendPoints(QVector<QPointF> point)
{
    for (auto it : point)
        appendPoint(it);
}

void SSeries::appendPoint(QPointF point)
{
    if (m_data.size() >= m_maxPointCount)
        m_data.remove(m_data.firstKey());
    m_data.insert(point.x(), point.y());
}

void SSeries::appendPoint(double x, double y, bool bForward)
{
    if (bForward)
        forwardAppendPoint(QPointF{x, y});
    else
        appendPoint(QPointF{x, y});
}

void SSeries::appendPoints(QVector<QPoint> point)
{
    for (auto it : point)
        appendPoint(it);
}

void SSeries::appendPoint(QPoint point)
{
    appendPoint(QPointF{point});
}

void SSeries::forwardAppendPoint(QPointF point)
{
    if (m_data.size() >= m_maxPointCount)
        m_data.remove(m_data.lastKey());
    m_data.insert(point.x(), point.y());
}

//void SSeries::insertPoint(QPoint point)
//{
//    insertPoint(QPointF{point});
//}

//void SSeries::insertPoint(QPointF point)
//{
//    if (m_data.size() >= m_maxPointCount)
//        m_data.replace(point.x(), point.y());
//    else
//        m_data.insert(point.x(), point.y());
//}

void SSeries::setPoints(QVector<QPointF> points)
{
    QMultiMap<double, double> t;
    for (auto point:points)
        t.insert(point.x(), point.y());
    m_data.swap(t);
}

void SSeries::setPoints(QVector<QPoint> points)
{
    QMultiMap<double, double> t;
    for (auto point:points)
        t.insert(point.x(), point.y());
    m_data.swap(t);
}

QPointF SSeries::at(int index)
{
    auto keys = m_data.uniqueKeys();
    return {keys[index], m_data.value(keys[index])};
}

int SSeries::count()
{
    return m_data.uniqueKeys().size();
}

// 线性图
SLineSeries::SLineSeries(QQuickItem* parent)
    : SSeries{parent}
{
    seriesType = SSeriesType::LineSeries;
}


// 散点图
SScatterSeries::SScatterSeries(QQuickItem* parent)
    : SSeries{parent}
{
    seriesType = SSeriesType::ScatterSeries;
}

