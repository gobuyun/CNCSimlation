#include "OpenglItem.hpp"

#include <QOpenGLFramebufferObjectFormat>

namespace  OpenglItem {

// render pipeline
void RenderProcess::createRenderTarget(const QSize &size)
{
    RenderContent* renderContent = nullptr;
    auto code = initRenderContent(&renderContent, size);
    if (code == 0)
    {
        m_renderContent.reset(renderContent);
    }
}

QOpenGLFramebufferObject *RenderProcess::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);

    auto fbo = new QOpenGLFramebufferObject(size, format);
    if (fbo) 
    {
        if (m_renderContent == nullptr)
            createRenderTarget(size);
        else
            m_renderContent->onResize(size.width(), size.height());
    }

    return fbo;
}

void RenderProcess::synchronize(QQuickFramebufferObject *item)
{
     auto wrap = dynamic_cast<OpenglItem*>(item);

     if (!m_renderContent)
         return;

     // 键盘事件
     // if (wrap->m_dirtySet.contains("keyboard"))
     // {
     //    m_renderContent->onKeyboardInput(wrap->m_keyboardCmdQue);
     //    wrap->m_dirtySet.remove("keyboard");
     // }
     // 镜头旋转
     if (wrap->m_dirtySet.contains("cameraRotate"))
     {
        m_renderContent->onMouseInput(0, wrap->m_mouseRotateData[0], wrap->m_mouseRotateData[1]);
        wrap->m_dirtySet.remove("cameraRotate");
     }
     // 镜头平移
     if (wrap->m_dirtySet.contains("cameraMove"))
     {
        m_renderContent->onMouseInput(1, wrap->m_mouseMoveData[0], wrap->m_mouseMoveData[1]);
        wrap->m_dirtySet.remove("cameraMove");
     }
     // 镜头缩放
     if (wrap->m_dirtySet.contains("cameraZoom"))
     {
         m_renderContent->onWheelInput(wrap->m_mouseWheelData[0], wrap->m_mouseWheelData[1]);
         wrap->m_dirtySet.remove("cameraZoom");
     }
}

void RenderProcess::render()
{
    if (m_renderContent)
    {
        m_renderContent->draw();
        update();
    }
}

int RenderProcess::initRenderContent(RenderContent** renderContent, const QSize &size)
{
    try
    {
        QStringList axisList = {
            "100F/xaxis.stl",
            "100F/yaxis.stl",
            "100F/zaxis.stl",
        };
        QStringList modelList = {
            "100F/base.stl"
        };

        *renderContent = new RenderContent(
            modelList,
            axisList,
            "shader/1.model_loading.vs", "shader/1.model_loading.fs",
            size.width(), size.height());
    }
    catch (const std::exception& e)
    {
        qDebug() << "std::exception: '" << e.what() << "'";
        return -1;
    }
    catch (...)
    {
        qDebug() << "Unexpected exception occurred!";
        return -1;
    }
    return 0;
}

OpenglItem::OpenglItem(QQuickItem *parent)
    : QQuickFramebufferObject{parent}
{
}

// void OpenglItem::keyboardCmd(int keyCode)
// {
//     // return;
//     m_keyboardCmdQue = keyCode;
//     m_dirtySet.insert("keyboard");
//     update();
// }

void OpenglItem::mouseCmd(int cmd, float deltaX, float deltaY)
{
    if (cmd == 0)
    {
        m_mouseMoveData = {deltaX, deltaY};
        m_dirtySet.insert("cameraMove");
    }
    else if (cmd == 1)
    {
        m_mouseRotateData = {deltaX, deltaY};
        m_dirtySet.insert("cameraRotate");
    }
    else if (cmd == 2)
    {
        m_mouseWheelData = {deltaX, deltaY};
        m_dirtySet.insert("cameraZoom");
    }
    update();
}

}

