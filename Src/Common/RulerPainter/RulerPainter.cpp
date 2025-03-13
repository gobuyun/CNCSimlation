#include "RulerPainter.h"

#include <QPainter>

RulerPainter::RulerPainter(QQuickItem *parent)
    : QQuickPaintedItem{parent}
{
    setAntialiasing(true);
}

void RulerPainter::paint(QPainter *painter)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(m_color);
    painter->setPen(pen);

    QFont font;
    font.setFamily("MiSans");
    font.setPixelSize(12);
    font.setWeight(500);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0);
    painter->setFont(font);

    QVector<QLineF> lineDatas;

    const auto BASEUNIT = 10; // 10 mm, fov = 90 // 每個刻度的間隔，單位mm
    const auto computeLongShowInterval = 5 + (5-((int)(m_zoom/0.2)))*5; // 計算長刻度間隔，在放大時就擴大間隔縮小時間隔不變
    const auto LONGSHOWINTERVAL = computeLongShowInterval<=0? 5:computeLongShowInterval; // 長刻度間隔
    const auto SHORTSHOWINTERVAL = LONGSHOWINTERVAL/5; // 短刻度間隔

    const auto SHORTLEN = 10; // uv coord
    const auto LONGLEN = 18;

    const auto c1 = qRound64(m_zoom*100); // 減少誤差
    const auto c2 = int(c1*BASEUNIT/100); // 單位長度所佔的像素個數

    const auto point1Y = m_directionY<0?height():0;
    const auto shortPoint2Y = m_directionY<0?height()-SHORTLEN:SHORTLEN;
    const auto longPoint2Y = m_directionY<0?height()-LONGLEN:LONGLEN;
    const auto textStartY = m_directionY<0?0:LONGLEN;

    for (int i = 1; i<width()+1; ++i)
    {
        double dial = (double)(i*1.0/c2);
        if (int(dial) == dial)
        {
            if (int(dial)%LONGSHOWINTERVAL == 0)
            {
                // long line
                const auto targetX = m_directionX<0? width() - i:i;
                lineDatas.push_back({targetX, point1Y,
                                     targetX, longPoint2Y});
                auto showText = QString::number(int(dial)*BASEUNIT);
                int strwidth = painter->fontMetrics().horizontalAdvance(showText);//字符串显示的像素大小
                painter->drawText(QRectF{targetX - strwidth/2, (double)textStartY, (double)strwidth, 20}
                                  ,Qt::AlignHCenter | Qt::AlignTop
                                  ,showText);
            }
            else if (int(dial)%SHORTSHOWINTERVAL == 0)
            {
                // short line
                const auto targetX = m_directionX<0? width() - i:i;
                lineDatas.push_back({targetX, point1Y,
                                     targetX, shortPoint2Y});
            }
        }
    }
    painter->drawLines(lineDatas);
}

