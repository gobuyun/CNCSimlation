#ifndef SCENE_HPP
#define SCENE_HPP

#include <string.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "BoundingBox.hpp"

class Scene
{
public:
    Scene(std::string xmlPath)
    {
        loadSceneByXml(xmlPath);
    }

    void loadSceneByXml(std::string xmlPath)
    {
        m_xmlPath = xmlPath;
        // parser xml
        if (parserXML(xmlPath, m_sceneDefineInfo) == false)
            return;

        // load model
        // std::string modelPath = "";
        // auto model = new Model(modelPath);
        // m_boundingBox = m_boundingBox + model->box;
        // m_modelDatas.push_back(ModelAssembleData{glm::mat4x4(1.0f), model});
    }

    Boundingbox m_boundingBox;

private:
    struct ModelAssembleData{
        glm::mat4x4 transformMat = glm::mat4x4(1.0f);
        Model* model;
    };
    struct SceneDefinedInfo{

    };

    bool parserXML(const std::string& xmlPath, SceneDefinedInfo& sceneInfo)
    {
        // xmlPath
        return false;
    }

private:
    std::string m_xmlPath;
    std::vector<ModelAssembleData> m_modelDatas;
    SceneDefinedInfo m_sceneDefineInfo;
};

#endif // SCENE_HPP
