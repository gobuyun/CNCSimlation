#ifndef COLORPICKERHELPER_H
#define COLORPICKERHELPER_H

#include <QObject>
#include <QVariant>
#include <QImage>


class ColorPickerHelper : public QObject
{
    Q_OBJECT
public:
    explicit ColorPickerHelper(QObject *parent = nullptr);

public slots:
//    void setCacheImage(QVariant imgVar); // 設置烘焙過的顏色選擇區域
    QColor pickerColorFromImage(int x, int y); // 從烘焙過的cache image採集實際顏色值
    QPoint findPixelPos(QColor keyColor); // 搜索像素點的位置

signals:

private:
    QImage m_pColorPickerArea;
};

#endif // COLORPICKERHELPER_H
