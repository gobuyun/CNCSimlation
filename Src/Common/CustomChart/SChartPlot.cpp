#include "SChartPlot.h"

#include <QPainter>
#include <QQuickWindow>

using namespace SChart;

SChartPlot::SChartPlot(QQuickItem* parent)
    : QQuickPaintedItem{parent}
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setAntialiasing(true);
}

void SChartPlot::paint(QPainter *painter)
{
    drawBackgroup(painter);
    drawAllSeries(painter);
}

SSeries *SChartPlot::createSeries(SSeriesType seriesType, QString seriesName)
{
    SSeries* ins = nullptr;
    switch(seriesType)
    {
    case SSeriesType::LineSeries: {
        ins = new SLineSeries(this);
    }
    break;
    case SSeriesType::ScatterSeries: {
        ins = new SScatterSeries(this);
    }
    break;
    default: {
        qDebug () << "SeriesType not support!";
        Q_ASSERT(false);
    }
    }
    if (ins)
        ins->setObjectName(seriesName);
    return ins;
}

void SChartPlot::removeSeries(SSeries *seriesIns)
{
    seriesIns->setParentItem(nullptr);
    seriesIns->deleteLater();
}

QPointF SChartPlot::getSeriesData(SSeries *seriesIns, double x, double y)
{
    auto val = mapPos2Val({x, y});
    val.setX(int(val.x()));
    auto& datas = seriesIns->pointDatas();
    auto it = datas.find(val.x());
    if (it != datas.end())
        return {val.x(), *it};
    return {-1, -1};
}

void SChartPlot::drawBackgroup(QPainter *painter)
{
    // grid
    painter->save();
    QPen penGrid;
    penGrid.setWidth(2);
    penGrid.setStyle(Qt::DashLine);
    penGrid.setColor(QColor(m_gridColor));
    painter->setPen(penGrid);
    auto xGridSpacing = int(width())/m_xTick;
    auto yGridSpacing = int(height())/m_yTick;
    QVector<QLine> grid;
    for (auto i = 0; i<m_xTick; ++i)
    {
        auto sx = (i+1)*xGridSpacing;
        grid.append({{sx, 0}, {sx, int(height())}});
    }
    for (auto i = 0; i<m_yTick; ++i)
    {
        auto sy = int(height()) - (i+1)*yGridSpacing;
        grid.append({{0, sy}, {int(width()), sy}});
    }
    painter->drawLines(grid);
    painter->restore();

    // Axis
    painter->save();
    penGrid.setWidth(2);
    penGrid.setStyle(Qt::SolidLine);
    painter->setPen(penGrid);
    painter->drawLine(QLine{{0, 0}, {0, int(height())}});
    painter->drawLine(QLine{{0, int(height())}, {int(width()), int(height())}});
    painter->restore();
}

QMap<int, QVector<SSeries*>> SChartPlot::findAllChildSeriesObj()
{
    auto items = childItems();
    QMap<int, QVector<SSeries*>> zOrder2series;
    for (auto it : items)
    {
        auto seriesIns = qobject_cast<SSeries*>(it);
        if (seriesIns)
        {
            auto zOrder = seriesIns->z();
            auto iter = zOrder2series.find(zOrder);
            if (iter == zOrder2series.end())
                zOrder2series.insert(zOrder, {});
            zOrder2series[zOrder].append(seriesIns);
        }
    }
    return zOrder2series;
}

QVector<QPointF> SChartPlot::mapValue2PaintData(const QMultiMap<double, double>& points)
{
    QVector<QPointF> paintLines;
//    int startInx = -1;
//    int endInx = -1;

    int xMin = m_xMin;
    int xMax = m_xMax;
    // find startInx
//    for (auto i = xMin; i<=xMax; ++i)
//    {
//        if (points.find(i) != points.end())
//        {
//            startInx = i;
//            break;
//        }
//    }
//    // find endInx
//    for (auto i = xMax; i>=xMin; --i)
//    {
//        if (points.find(i) != points.end())
//        {
//            endInx = i;
//            break;
//        }
//    }
//    // no vaild value
//    if (startInx == -1 || endInx == -1)
//        return paintLines;

    auto keys = points.uniqueKeys();
    for (auto it : keys)
    {
        if (it < xMin || it > xMax)
            continue;
        auto xVal = it;
        foreach (auto yValIt, points.values(it))
            paintLines.append(mapVal2Pos({(double)xVal, yValIt}));
    }


//    for (auto i = startInx; i<=endInx; ++i)
//    {
//        auto it = points.find(i);
//        if (it == points.end())
//            continue;
//        auto xVal = i;
//        foreach (auto yValIt, points.values(i))
//            paintLines.append(mapVal2Pos({(double)xVal, yValIt}));
//    }
    return paintLines;
}

QPointF SChartPlot::mapVal2Pos(QPointF point)
{
    auto xPos = qAbs(point.x() - m_xMin)/qAbs(m_xMax - m_xMin)*width();
    auto yPos = height() - (qAbs(point.y() - m_yMin)/qAbs(m_yMax - m_yMin)*height());
    return {xPos, yPos};
}

QPointF SChartPlot::mapPos2Val(QPointF pos)
{
    auto xVal = pos.x()/width()*qAbs(m_xMax - m_xMin) + m_xMin;
    auto yVal = (height() - pos.y())/height()*qAbs(m_yMax - m_yMin) + m_yMin;
    return {xVal, yVal};
}

void SChartPlot::drawAllSeries(QPainter *painter)
{
    if (m_xMax == m_xMin || m_yMax== m_yMin)
        return;

    auto zOrder2series = findAllChildSeriesObj();
    auto zOrderList = zOrder2series.keys();
    for (auto zOrder : zOrderList)
    {
        auto seriesVec = zOrder2series[zOrder];
        for (auto series : seriesVec)
        {
            if (!series->isVisible())
                continue;
            if (series->seriesType == SSeriesType::LineSeries)
                drawLineSeries( painter, qobject_cast<SLineSeries*>(series) );
            else if (series->seriesType == SSeriesType::ScatterSeries)
                drawScatterSeries( painter, qobject_cast<SScatterSeries*>(series) );
        }
    }
}

void SChartPlot::drawLineSeries(QPainter *painter, SLineSeries* lineSeries)
{
    QPen pen;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(lineSeries->getlineStyle());
    pen.setWidth(lineSeries->getpenWidth());
    pen.setColor(lineSeries->getcolor());
    painter->setPen(pen);
    painter->drawPolyline(mapValue2PaintData(lineSeries->pointDatas()));
    painter->restore();
}

void SChartPlot::drawScatterSeries(QPainter *painter, SScatterSeries* scatterSeries)
{
    auto points = mapValue2PaintData(scatterSeries->pointDatas());
    QPen pen;
    for (auto point : points)
    {
        auto cx = point.x();
        auto cy = point.y();
        // 画出外圈
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        auto outterDia = scatterSeries->getradius()*2;
        pen.setWidth(2);
        pen.setColor(QColor(scatterSeries->getcolor()));
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::white);
        painter->drawEllipse(QPointF{cx, cy}, outterDia, outterDia);
        painter->restore();
        // 画出内圈
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        auto innerDia = outterDia*0.6;
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(QColor(scatterSeries->getcolor()));
        painter->setBrush(QColor(scatterSeries->getcolor()));
        painter->drawEllipse(QPointF{cx, cy}, innerDia, innerDia);
        painter->restore();
    }
}

//void SChartPlot::drawTimeStamp(QPainter *painter)
//{
//    painter->save();
//    painter->setPen(Qt::black);
//    auto y = height() - painter->font().pixelSize();
//    auto tick = 5;
//    auto itemWidth = width()/tick;
//    auto startTime = ts[0];
//    auto timeOffset = (ts[ts.size()-1] - ts[0])/tick;
//    for (auto i = 0; i<tick; ++i)
//    {
//        painter->drawText(i*itemWidth, y, QString::number(startTime+timeOffset*i));
//    }
//    painter->restore();
//}

//void SChartPlot::drawValue(QPainter *painter)
//{
//    painter->save();
//    painter->restore();
//}

