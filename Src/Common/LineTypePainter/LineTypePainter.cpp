#include "LineTypePainter.h"

#include <QPainter>
#include <QPen>

LineTypeItemPainter::LineTypeItemPainter()
{
    connect(this, &LineTypeItemPainter::lineTypeChanged, [this](){
        update();
    });
    connect(this, &LineTypeItemPainter::colorChanged, [this](){
        update();
    });
}

void LineTypeItemPainter::paint(QPainter *painter)
{
    QPen pen;
    pen.setStyle((Qt::PenStyle)m_lineType);
    pen.setWidthF(height());
    pen.setColor(m_color);
    painter->setPen(pen);
    painter->drawLine(QLine{QPoint(0, 0), QPoint(width(), 0)});
}
