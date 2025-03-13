#ifndef RULERPAINTER_H
#define RULERPAINTER_H

#include <QQuickPaintedItem>
#include <QQuickItem>

#include "../CommonDef.hpp"

class RulerPainter : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
    QML_DECL_PROPERTY(double, zoom, 1.0f)
    QML_DECL_PROPERTY(QString, color, "#000000")
    QML_DECL_PROPERTY(int, directionX, 1)
    QML_DECL_PROPERTY(int, directionY, -1)

public:
    RulerPainter(QQuickItem *parent = nullptr);

    virtual void paint(QPainter *painter) override;
};

#endif // RULERPAINTER_H
