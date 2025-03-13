#ifndef LINETYPEPAINTER_H
#define LINETYPEPAINTER_H

#include <QQuickPaintedItem>
#include <QObject>

#include "../CommonDef.hpp"


// 繪製Qt::PenStyle到item上
class LineTypeItemPainter : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    QML_DECL_PROPERTY(int, lineType, Qt::SolidLine)
    QML_DECL_PROPERTY(QString, color, "#000000")

public:
    LineTypeItemPainter();

    virtual void paint(QPainter *painter) override;

private:

};

#endif // LINETYPEPAINTER_H
