#include "ColorPickerHelper.h"

#include <QDebug>

ColorPickerHelper::ColorPickerHelper(QObject *parent)
    : QObject{parent}
{
    m_pColorPickerArea = QImage(":/images/colorPicker/ColorPickerArea.png");
}

//void ColorPickerHelper::setCacheImage(QVariant imgVar)
//{
//    m_pColorPickerArea = imgVar.value<QImage>();
//}

QColor ColorPickerHelper::pickerColorFromImage(int x, int y)
{
    if (m_pColorPickerArea.isNull())
        return QColor();

    auto pickerColor = m_pColorPickerArea.pixelColor(x > 0? x-1: x,
                                                     y > 0? y-1: y);
    pickerColor.setAlpha(255);
    if (x == 0) pickerColor.setRgb(255, 255, 255); // 第一列強制爲白色，因爲顏色選擇圖帶圓角
    return pickerColor;
}

QPoint ColorPickerHelper::findPixelPos(QColor keyColor)
{
    QPoint res(-1, -1); // 無效搜索值

    if (m_pColorPickerArea.isNull())
        return res;

    keyColor.setAlpha(255); // 搜索圖像不考慮透明通道
    // 因爲使用的顏色選擇區圖片是帶半徑的，因此需要對一些邊界顏色做處理
    if (keyColor.hue() == -1)
    {
        // black
        if (keyColor.value() == 0)
        {
            return QPoint(m_pColorPickerArea.width()-1, 0);
        }
        // white
        if (keyColor.value() == 255)
        {
            return QPoint(0, 0);
        }
    }
    auto startFindLineInx = keyColor.hue()/255 * m_pColorPickerArea.height(); // 根據色相算起始行號
    startFindLineInx = startFindLineInx == 0? startFindLineInx:startFindLineInx-1; // constScanLine(index) index從0開始
    auto pStartScanLine = (QRgb*)m_pColorPickerArea.constScanLine(startFindLineInx);
    auto p = pStartScanLine;
    auto remainLine = m_pColorPickerArea.height() - startFindLineInx; // 剩餘搜索行數
    if (remainLine < 0)
        return res;
    auto remainPixel = remainLine*m_pColorPickerArea.width(); // 剩餘搜索像素點
    while (remainPixel)
    {
        if (keyColor.toRgb() == QColor(*p))
        {
            auto offset = p - pStartScanLine;
            res.setX(offset%m_pColorPickerArea.width());
            res.setY(startFindLineInx + offset/m_pColorPickerArea.width());
            qDebug() << "Find:" << res << QColor(*p);
            break;
        }
        ++p;
        --remainPixel;
    }
    return res;
}

