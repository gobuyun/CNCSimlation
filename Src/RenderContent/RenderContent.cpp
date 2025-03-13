#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderContent.h"
#include "../Base/shader.hpp"
#include "../Base/camera.h"
#include "../Base/model.h"
#include "../Base/Scene.hpp"
//#include "GL/GLCamera.h"

// Rail
RenderContent::RenderContent(QStringList modelPathList,
                             QStringList axisPath,
                             QString toolPath,
                             QVector<QVector<int> > axisHierarchy,
                             std::string vertexShaderPath,
                             std::string fragShaderPath,
                             unsigned long width,
                             unsigned long height)
{
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    m_scene = new Scene("");

    m_axisHierarchy.swap(axisHierarchy);

    // non-axis
    foreach(auto path, modelPathList)
    {
        auto model = new Model(path.toStdString());
        m_models.append(model);
        m_boundingBox = m_boundingBox + model->box;
    }
    // tool
    m_toolModel = new Model(toolPath.toStdString(), 8.0f);
    m_boundingBox = m_boundingBox + m_toolModel->box;
    // axis
    foreach(auto path, axisPath)
    {
        auto model = new Model(path.toStdString());
        m_axisModels.append(model);
        m_boundingBox = m_boundingBox + model->box;
    }
    auto center = m_boundingBox.center();
    m_camera.reset(new Camera(glm::vec3(center.x, center.y, center.z)));
    m_camera->setDistance(m_boundingBox.diagonalLength());
    m_shader.reset(new Shader(vertexShaderPath.c_str(), fragShaderPath.c_str()));

    onResize(width, height);
}

RenderContent::~RenderContent()
{

}

int RenderContent::draw()
{
    using namespace MachineControl;

    // update motion data
    MachineControl::getAxisCurVal(m_curAxisVal);

    // clear last cache
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // actived shader
    m_shader->use();

    // view/projection transformations
    int halfH = ((m_height*1.0)/2)/m_camera->Zoom;
    double ratio = m_width*1.0/m_height;
    int halfW = halfH*ratio;
    static auto farPlane = m_boundingBox.diagonalLength()/m_camera->Zoom;
//    qDebug() << m_camera->Position.x  << m_camera->Position.y << m_camera->Position.z;
    glm::mat4 projection = glm::ortho<float>(-halfW, halfW, -halfH, halfH, 1.0, farPlane);
    glm::mat4 view = m_camera->GetViewMatrix();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);

    std::vector<glm::vec3> direction = {{0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};

    foreach(auto model, m_models)
    {
        glm::mat4 modelMat = glm::mat4(1.0f);
        m_shader->setMat4("model", modelMat);
        m_shader->setInt("intersects", 0);
        model->Draw(*m_shader);
    }
    std::vector<glm::vec3> axisTranslateCache;
    for (auto it = 0; it<m_axisModels.size(); ++it)
    {
        // render the loaded model
        auto pos = MachineControl::INVAILD_POS;
        if (!m_curAxisVal.empty())
            pos = m_curAxisVal.at(it);
        // auto speed = modelData.at(it).speed;
        auto curDiection = direction.at(it);
        auto offset = glm::vec3(curDiection[0], curDiection[1], curDiection[2]);
        if (pos != MachineControl::INVAILD_POS)
            offset = glm::vec3(offset[0]*pos, offset[1]*pos, offset[2]*pos);
        axisTranslateCache.push_back(offset);
    }

    // 碰撞检测、以及渲染参数准备
    int intersectsFlag[3] = {0};
    glm::mat4 modelMat[3];
    glm::vec3 zAxisOffset;
    for (auto it = 0; it<m_axisModels.size(); ++it)
    {
        auto parentHierarchys = m_axisHierarchy[it];

        auto model = m_axisModels.at(it);
        // render the loaded model
        modelMat[it] = glm::mat4(1.0f);
        auto offset = axisTranslateCache[it];
        foreach(auto hierarchy, parentHierarchys)
        {
            auto parentTranslate = axisTranslateCache[hierarchy];
            offset += parentTranslate;
        }
        modelMat[it] = glm::translate(modelMat[it], offset); // translate it down so it's at the center of the scene
        modelMat[it] = glm::rotate(modelMat[it], 0.0f, glm::vec3(1.00f, 1.000f, 1.000f));	// it's a bit too big for our scene, so scale it down

        model->m_aabbBox->Transform({offset.x, offset.y, offset.z}, {0, 0, 0},true);
        if (it == m_axisModels.size()-1)
            zAxisOffset = offset;
    }
    // tool碰撞計算
    glm::mat4 modelToolMat;
    glm::vec3 toolOffset = {212, 1395, 858};
    toolOffset += zAxisOffset;
    m_toolModel->m_aabbBox->Transform({toolOffset.x, toolOffset.y, toolOffset.z}, {-90, 0.0, 0.0},true);
    intersectsFlag[0] = m_toolModel->m_aabbBox->IsIntersects(m_axisModels[0]->m_aabbBox); // 与x比较
    intersectsFlag[1] = m_toolModel->m_aabbBox->IsIntersects(m_axisModels[1]->m_aabbBox); // 与Y比较
    intersectsFlag[2] = 0;
    int toolintersects = intersectsFlag[0] || intersectsFlag[1] || intersectsFlag[2];
    modelToolMat = glm::translate(modelToolMat, toolOffset); // translate it down so it's at the center of the scene
    modelToolMat = glm::rotate(modelToolMat, glm::radians(-90.0f), glm::vec3(1.00f, .000f, 0.000f));	// it's a bit too big for our scene, so scale it down
    m_shader->setMat4("model", modelToolMat);
    m_shader->setInt("intersects", toolintersects);
    m_toolModel->Draw(*m_shader);
    m_shader->setMat4("model", glm::mat4(1.0f));
//    m_toolModel->DrawBox(*m_shader);

    for (auto it = 0; it<m_axisModels.size(); ++it)
    {
        auto model = m_axisModels.at(it);
        m_shader->setMat4("model", modelMat[it]);
        m_shader->setInt("intersects", intersectsFlag[it]);
        model->Draw(*m_shader);

        m_shader->setMat4("model", glm::mat4(1.0f));
//        model->DrawBox(*m_shader);
    }


    return 0;
}

void RenderContent::onKeyboardInput(int keyCode)
{
     auto speed = (decltype(m_deltaTime))100;
     speed = 1.0f;
     m_deltaTime = 1.0f;
     if (keyCode == Qt::Key_W)
         m_camera->ProcessKeyboard(FORWARD, m_deltaTime*speed);
     if (keyCode == Qt::Key_S)
         m_camera->ProcessKeyboard(BACKWARD, m_deltaTime*speed);
     if (keyCode == Qt::Key_A)
         m_camera->ProcessKeyboard(LEFT, m_deltaTime*speed);
     if (keyCode == Qt::Key_D)
         m_camera->ProcessKeyboard(RIGHT, m_deltaTime*speed);
}

void RenderContent::onResize(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    
    m_width = width;
    m_height = height;

    m_camera->setBaseZoom(m_height/m_boundingBox.diagonalLength());
}

void RenderContent::onMouseInput(int typeID, double xposIn, double yposIn)
{
    if (typeID == 0)
        m_camera->ProcessMouseRotate(xposIn, yposIn);
    else
        m_camera->ProcessMouseMove(xposIn, yposIn);
}

void RenderContent::onWheelInput(double xoffset, double yoffset)
{
    m_camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

