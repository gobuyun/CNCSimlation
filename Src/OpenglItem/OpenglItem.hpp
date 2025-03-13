#ifndef OPENGLITEM_H
#define OPENGLITEM_H

// 文件为适配层次，目的是适配qml下，统一启用与不启用module works模块的表现
#include <QQuickFramebufferObject>
#include <QQuickItem>

#include <array>

#include "../Common/CommonDef.hpp"
#include "../RenderContent/RenderContent.h"

namespace OpenglItem {

class RenderProcess : public QQuickFramebufferObject::Renderer
{
public:
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

protected:
    virtual void render() override;
    virtual void synchronize(QQuickFramebufferObject *item) override;

private:
    int initRenderContent(RenderContent** renderContent, const QSize &size);
    void createRenderTarget(const QSize &size);

private:
    std::unique_ptr<RenderContent> m_renderContent = nullptr;
    float zoom;
    float angle = 0;
    QVector<int> posDelta;
    int machineType = 0;
};

class OpenglItem : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_DECL_PROPERTY(std::vector<double>, curAxisVal, std::vector<double>(0.0, 3))
public:
    OpenglItem(QQuickItem *parent = nullptr);

    QVector<QVector<double>> popAxisData();

public slots:
    // void keyboardCmd(int keyCode);
    void mouseCmd(int cmd, float deltaX, float deltaY);
    void handCmd(unsigned int jog_id, int jog_dir);
    void positionCmd(std::vector<double> axisPe);
    void autoCmd(QStringList axisvAalStr);
    bool autoRunCanPush();
    void stopAutoCmd();
    void updateCurAxisVal();

public:
    virtual QQuickFramebufferObject::Renderer *createRenderer() const override
    {
        return new RenderProcess;
    }

public:
    int m_keyboardCmdQue;
    std::array<float, 2> m_mouseMoveData;
    std::array<float, 2> m_mouseRotateData;
    std::array<float, 2> m_mouseWheelData;

    QSet<QString> m_dirtySet;

private:
    QVector<QVector<double>> m_axisValues;
};


// 注册
inline void registerModule()
{
    auto packName = "OpenglExt";
    qmlRegisterType<OpenglItem>(packName, 1, 0, "OpenglItem");
}

}

#endif // OPENGLITEM_H
