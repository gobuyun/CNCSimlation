#ifndef RENDERCONTENT_H
#define RENDERCONTENT_H


#include <memory>
#include <string>

#include <QObject>

#include "../Base/BoundingBox.hpp"
#include "MachineControl/MachineControl.hpp"

class Camera;
class Shader;
class Model;
class Scene;


class RenderContent {
public:
    RenderContent(QStringList modelPath,
        QStringList axisPath,
        QString toolPath,
        QVector<QVector<int>> axisHierarchy,
        std::string vertexShaderPath,
        std::string fragShaderPath,
        unsigned long width,
        unsigned long height);
    ~RenderContent();

public:
    // expose API
    int draw();
    void onKeyboardInput(int keyCode);
    void onResize(unsigned int width, unsigned int height);
    void onMouseInput(int typeID, double xposIn, double yposIn);
    void onWheelInput(double xoffset, double yoffset);

private:
    // canvas size
    unsigned int m_width = 0;
    unsigned int m_height = 0;

    // camera
    std::unique_ptr<Camera> m_camera = nullptr;
    float m_lastX = m_width / 2.0f;
    float m_lastY = m_height / 2.0f;
    bool m_firstMouse = true;

    // shader
    std::unique_ptr<Shader> m_shader = nullptr;
    // model
    QVector<Model*> m_models;
    QVector<Model*> m_axisModels;
    Model* m_toolModel;
    QVector<QVector<int>> m_axisHierarchy;
    Boundingbox m_boundingBox;
    Scene* m_scene;

    std::vector<double> m_curAxisVal;

    // timing
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
};
#endif // RENDERCONTENT
