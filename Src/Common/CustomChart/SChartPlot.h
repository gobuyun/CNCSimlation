#ifndef SCHARTPLOT_H
#define SCHARTPLOT_H

#include <QHash>
#include <QQuickPaintedItem>
#include <QTimer>

#include "SSeries.h"


namespace SChart {
class SChartPlot : public QQuickPaintedItem
{
    Q_OBJECT
    QML_DECL_PROPERTY(double, xMax, 0.0)
    QML_DECL_PROPERTY(double, xMin, 0.0)
    QML_DECL_PROPERTY(double, yMax, 0.0)
    QML_DECL_PROPERTY(double, yMin, 0.0)
    QML_DECL_PROPERTY(int, xTick, 10) // x轴分片
    QML_DECL_PROPERTY(int, yTick, 10) // y轴分片
    // 网格属性
    QML_DECL_PROPERTY(QString, gridColor, "#C5C5C5")

public:
    SChartPlot(QQuickItem* parent = nullptr);

    virtual void paint(QPainter *painter) override;

public slots:
    SChart::SSeries* createSeries(SChart::SSeriesType seriesType,
                                  QString seriesName);
    void removeSeries(SChart::SSeries* seriesIns);
    QPointF getSeriesData(SChart::SSeries* seriesIns, double x, double y);
    QPointF mapVal2Pos(QPointF point); // 坐标坐标点 =》屏幕坐标点
    QPointF mapPos2Val(QPointF pos); // 屏幕坐标点 =》坐标坐标点

private:
    void drawBackgroup(QPainter *painter);

    QVector<QPointF> mapValue2PaintData(const QMultiMap<double, double> &points); // 把series数据转为绘制坐标数据
    QMap<int, QVector<SSeries*>> findAllChildSeriesObj();
    void drawAllSeries(QPainter *painter);
    void drawLineSeries(QPainter *painter, SLineSeries *lineSeries);
    void drawScatterSeries(QPainter *painter, SScatterSeries *scatterSeries);
//    void drawTimeStamp(QPainter *painter);
//    void drawValue(QPainter *painter);

private:

};

// 注册
inline void registerModule()
{
    auto packName = "SChart";
    qmlRegisterType<SSeries>(packName, 1, 0, "SSeries");
    qmlRegisterType<SLineSeries>(packName, 1, 0, "SLineSeries");
    qmlRegisterType<SScatterSeries>(packName, 1, 0, "SScatterSeries");
    qmlRegisterType<SChartPlot>(packName, 1, 0, "SChartPlot");

    qmlRegisterUncreatableMetaObject(staticMetaObject, packName, 1, 0, "SeriesType", "Access to enums & flags only");

    qRegisterMetaType<SChart::SSeriesType>("SChart::SSeriesType");
}

}
#endif // SCHARTPLOT_H
