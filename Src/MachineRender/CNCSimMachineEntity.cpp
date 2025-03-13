// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "CNCSimMachineEntity.hpp"

#include "GeometryNodeEntity.hpp"
#include "MachineGeometryNodeEntity.hpp"
#include "StockNodeEntity.hpp"
#include "ToolNodeEntity.hpp"
#include "mwMachineDefinitionNode.hpp"
#include "mwMachineGeometryNode.hpp"
#include "mwMachineItemNode.hpp"
#include "mwToolNode.hpp"
#include "OwnVisualizationSample.hpp"

#include "../../Common/StbImage/stb_image.h"

#include <QCoreApplication>
#include <iostream>

namespace mwCNCSimVisualizationSamples
{

CNCSimMachineEntity::CNCSimMachineEntity(const misc::mwAutoPointer<cncsim::mwEngine>& engine)
	: m_Engine(engine), m_Initialized(false)
{
    initializeOpenGLFunctions();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_fAngle = 0;
    m_fScale = 1;
}

CNCSimMachineEntity::~CNCSimMachineEntity()
{
	RemoveAllEntities();
}

/// [PrepareForDrawing]
void CNCSimMachineEntity::PrepareForDrawing()
{
	std::lock_guard<std::mutex> entitiesAccessLock(m_EntitiesAccessMutex);
	misc::mwAutoCriticalSection engineLock(m_Engine->GetKinematicTreeLock());

	// Clean up unneeded entities.
	DeleteRemovedEntities();

	// Prepare the vectors over which we will iterate when drawing
	m_PreparedUnicoloredEntities.clear();
	// Overhead for stock is usually neglectable, so we just reserve for all nodes...
	m_PreparedUnicoloredEntities.reserve(m_MachineEntities.size());
	// .. do not reserve for the stocks
	m_PreparedVertexColoredEntities.clear();

	// The current bounding box could have changed, so we will recalculate it
	m_VisualBoundingBox.Uninitialize();

	// Iterate over all entities, prepare them and update the new bounding box.
	typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
	EntityEntry currentEntry = m_MachineEntities.begin();
	for (; currentEntry != m_MachineEntities.end(); ++currentEntry)
	{
		// Prepare
		currentEntry->second->PrepareForDrawing();

		// Add to corresponding vector for easier usage later on
		if (currentEntry->second->IsVertexColored())
		{
			m_PreparedVertexColoredEntities.push_back(
				static_cast<VertexColoredNodeEntity*>(currentEntry->second));
		}
		else
		{
			m_PreparedUnicoloredEntities.push_back(
				static_cast<UnicoloredNodeEntity*>(currentEntry->second));
		}

		// Update bounding box
		m_VisualBoundingBox += currentEntry->second->GetVisualBoundingBox();
	}
}
/// [PrepareForDrawing]

const cadcam::mw3dBoundingBox<float>& CNCSimMachineEntity::GetVisualBoundingBox() const
{
	return m_VisualBoundingBox;
}

/// [Initialize]
void CNCSimMachineEntity::Initialize()
{
	if (m_Initialized)
	{
		throw std::logic_error("The drawer can only be intialized once.");
	}

	// Stop engine from changing
	misc::mwAutoCriticalSection engineLock(m_Engine->GetKinematicTreeLock());

	// Trigger all viewModels being created
	if (m_Engine->GetMachine().IsValidHandle())
	{
		AddEntitiesForSubtree(m_Engine->GetMachine());
	}

	// Set Initialized flag so that updates are processed
    m_Initialized = true;
    readyBgShader();
}

void CNCSimMachineEntity::RegisterOwnVisualObject(OwnVisualizationSample *sample)
{
    m_sample = sample;
}
/// [Initialize]

/// [Draw]
void CNCSimMachineEntity::Draw(
	const cadcam::mwHomogenousMatrix& projectionMat, const cadcam::mwHomogenousMatrix& viewMat)
{
    // draw background color
    drawBackground();

	// First draw unicolored entities
    m_unicoloredShader.Activate();
    const UnicoloredShader::UnicoloredShaderLocations& unicoloredLocations =
        m_unicoloredShader.GetShaderLocations();
    glUniform1f(unicoloredLocations.AmbientIntensityLocation, 0.2f);
    glUniformMatrix4fv(
        unicoloredLocations.ProjectionMatrixLocation,
        1,
        GL_TRUE,
        (const GLfloat*)projectionMat.GetData());
    for (UnicoloredNodeEntity* entity : m_PreparedUnicoloredEntities)
    {
        entity->Draw(unicoloredLocations, viewMat);
    }
    m_unicoloredShader.Deactivate();

    // Then the vertex colored entities
    m_vertexColoredShader.Activate();
    const VertexColoredShader::VertexColoredShaderLocations& vertexColoredLocations =
        m_vertexColoredShader.GetShaderLocations();
    glUniform1f(vertexColoredLocations.AmbientIntensityLocation, 0.2f);
    glUniformMatrix4fv(
        vertexColoredLocations.ProjectionMatrixLocation,
        1,
        GL_TRUE,
        (const GLfloat*)projectionMat.GetData());

    for (VertexColoredNodeEntity* entity : m_PreparedVertexColoredEntities)
    {
        entity->Draw(vertexColoredLocations, viewMat);
    }
    m_vertexColoredShader.Deactivate();
}

void CNCSimMachineEntity::readyBgShader()
{
    int width, height, nrChannels;
    misc::mwstring appPath = QCoreApplication::applicationDirPath().toStdString();
    appPath += "/Resources/simbgclr";
    m_bgTextureData = stbi_load(appPath.ToUTF8().c_str(), &width, &height, &nrChannels, 0);
    if (!m_bgTextureData)
    {
        return;
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_bgTextureData);

    const float zPlane = 1.0f;
    m_vertexData = {
        -1.0, -1.0, zPlane, 0, 0,
        -1.0, 1.0, zPlane, 0, 1,
        1.0, 1.0, zPlane, 1, 1,

        -1.0, -1.0, zPlane, 0, 0,
        1.0, -1.0, zPlane, 1, 0,
        1.0, 1.0, zPlane, 1, 1
    };

    glGenBuffers(1, &m_posVer);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);
}

void CNCSimMachineEntity::releaseBgResources()
{
    stbi_image_free(m_bgTextureData);
    glDeleteBuffers(1, &m_posVer);
    glDeleteTextures(1, &m_texture);
}

void CNCSimMachineEntity::drawBackground()
{ 
    glDepthFunc(GL_LEQUAL); // 渲染背景时直接渲染到远平面，所以开启equal
    s.Activate();

    const auto& bgShaderLocation = s.GetShaderLocations();
    // texture
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(bgShaderLocation.TextureLocation, 0);
    // vertex
    glBindBuffer(GL_ARRAY_BUFFER, m_posVer);
    glVertexAttribPointer(
        bgShaderLocation.VertexLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 5,
        (void*)nullptr);
    glVertexAttribPointer(
        bgShaderLocation.UVLocation,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 5,
        (void*)(3 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, (m_vertexData.size()/5)*3);

    s.Deactivate();
    glDepthFunc(GL_LESS);
}
/// [Draw]

/// [IterateSubtree]
void CNCSimMachineEntity::IterateSubtree(
	const cncsim::mwMachineItemNode& subtreeRoot,
	std::function<void(const cncsim::mwMachineItemNode&)> f)
{
	// First apply to the current node
	f(subtreeRoot);
    std::cout << "IterateSubtree" << std::endl;
	// After that we start a recursion on the children
	for (const cncsim::mwMachineItemNode& childNode : subtreeRoot.GetChildren())
	{
		IterateSubtree(childNode, f);
	}
}
/// [IterateSubtree]

/// [AddEntitiesForSubtree]
void CNCSimMachineEntity::AddEntitiesForSubtree(const cncsim::mwMachineItemNode& subtreeRoot)
{
    std::cout << "AddEntitiesForSubtree" << std::endl;
	std::lock_guard<std::mutex> entitiesLock(m_EntitiesAccessMutex);
	IterateSubtree(
		subtreeRoot, [&](const cncsim::mwMachineItemNode& node) { this->AddEntityForNode(node); });
}
/// [AddEntitiesForSubtree]

/// [AddEntityForNode]
void CNCSimMachineEntity::AddEntityForNode(const cncsim::mwMachineItemNode& node)
{
    std::cout << "AddEntityForNode" << std::endl;
	if (node.IsValidHandle())
	{
		if (auto mgNode = node.TryAsNode<cncsim::mwMachineGeometryNode>())
		{
			m_MachineEntities[node.GetId()] = new MachineGeometryNodeEntity(mgNode);
		}
		else if (auto toolNode = node.TryAsNode<cncsim::mwToolNode>())
		{
			m_MachineEntities[node.GetId()] = new ToolNodeEntity(toolNode);
		}
		else if (auto stockNode = node.TryAsNode<cncsim::mwStockNode>())
		{
			m_MachineEntities[node.GetId()] = new StockNodeEntity(stockNode);
		}
	}
	else
	{
		std::runtime_error("Tried adding entities for an invalid handle.");
	}
}
/// [AddEntityForNode]

/// [RemoveEntitiesForSubtree]
void CNCSimMachineEntity::RemoveEntitiesForSubtree(const cncsim::mwMachineItemNode& subtreeRoot)
{
    std::cout << "RemoveEntitiesForSubtree" << std::endl;
	std::lock_guard<std::mutex> entityLock(m_EntitiesAccessMutex);
	std::lock_guard<std::mutex> deletableEntitiesLock(m_DeletableEntitiesMutex);
	IterateSubtree(
		subtreeRoot,
		[&](const cncsim::mwMachineItemNode& node) { this->RemoveEntityForNode(node); });
}
/// [RemoveEntitiesForSubtree]

/// [RemoveEntityForNode]
void CNCSimMachineEntity::RemoveEntityForNode(const cncsim::mwMachineItemNode& node)
{
    std::cout << "RemoveEntityForNode" << std::endl;
	if (node.IsValidHandle())
	{
		typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
		EntityEntry entry = m_MachineEntities.find(node.GetId());
		if (entry != m_MachineEntities.end())
		{
			m_DeletableEntities.push_back(entry->second);
			m_MachineEntities.erase(entry);
		}
	}
	else
	{
		std::runtime_error("Tried removing entities for an invalid handle.");
	}
}
/// [RemoveEntityForNode]

void CNCSimMachineEntity::RemoveAllEntities()
{
    std::cout << "RemoveAllEntities" << std::endl;
	std::lock_guard<std::mutex> entitiesAccessLock(m_EntitiesAccessMutex);
	std::lock_guard<std::mutex> deletableEntitiesLock(m_DeletableEntitiesMutex);

	typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
	EntityEntry currentEntry = m_MachineEntities.begin();

	m_DeletableEntities.reserve(m_DeletableEntities.size() + m_MachineEntities.size());
	for (; currentEntry != m_MachineEntities.end(); ++currentEntry)
	{
		m_DeletableEntities.push_back(currentEntry->second);
	}

	m_MachineEntities.clear();
}

/// [DeleteRemovedEntities]
void CNCSimMachineEntity::DeleteRemovedEntities()
{
	std::lock_guard<std::mutex> deletableEntitiesLock(m_DeletableEntitiesMutex);

	typedef std::vector<GeometryNodeEntity*>::iterator RemoveEntry;
	RemoveEntry currentEntry = m_DeletableEntities.begin();
	for (; currentEntry != m_DeletableEntities.end(); ++currentEntry)
	{
        std::cout << "DeleteRemovedEntities" << std::endl;
		delete *currentEntry;
	}
	m_DeletableEntities.clear();
}
/// [DeleteRemovedEntities]

/// [OnMachineDefinitionChanged]
void CNCSimMachineEntity::OnMachineDefinitionChanged(
	cncsim::mwEngine& /*sender*/, cncsim::mwMachineDefinitionNode rootNode)
{
	if (!m_Initialized)
		return;

    std::cout << "OnMachineDefinitionChanged" << std::endl;
    RemoveAllEntities();

    if (!rootNode.IsValidHandle())
        return;
    AddEntitiesForSubtree(rootNode);
    m_sample->setRenderStart();
}
/// [OnMachineDefinitionChanged]

/// [OnSubTreeAdded]
void CNCSimMachineEntity::OnSubTreeAdded(
	cncsim::mwEngine& /*sender*/, cncsim::mwMachineItemNode subTreeRootNode)
{
	if (!m_Initialized)
		return;
std::cout << "OnSubTreeAdded" << std::endl;
	AddEntitiesForSubtree(subTreeRootNode);
}
/// [OnSubTreeAdded]

/// [OnSubTreeRemoved]
void CNCSimMachineEntity::OnSubTreeRemoved(
	cncsim::mwEngine& /*sender*/, cncsim::mwMachineItemNode subTreeRootNode)
{
	if (!m_Initialized)
		return;
std::cout << "OnSubTreeRemoved" << std::endl;
	RemoveEntitiesForSubtree(subTreeRootNode);
}
/// [OnSubTreeRemoved]

/// [OnVisualRepresentationChanged]
void CNCSimMachineEntity::OnVisualRepresentationChanged(
	cncsim::mwEngine& /*sender*/, cncsim::mwGeometryNode node)
{
	if (!m_Initialized)
		return;
std::cout << "OnVisualRepresentationChanged" << std::endl;
	std::lock_guard<std::mutex> entitiesAccessLock(m_EntitiesAccessMutex);
	typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
	EntityEntry entry = m_MachineEntities.find(node.GetId());
	if (entry != m_MachineEntities.end())
	{
		entry->second->NotifyVisualRepresentationChanged();
	}
}
/// [OnVisualRepresentationChanged]

void CNCSimMachineEntity::OnToolsChanged(cncsim::mwEngine& /*sender*/, cncsim::mwToolNode node)
{
	if (!m_Initialized)
		return;
std::cout << "OnToolsChanged" << std::endl;
	std::lock_guard<std::mutex> entitiesAccessLock(m_EntitiesAccessMutex);
	typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
	EntityEntry entry = m_MachineEntities.find(node.GetId());
	if (entry != m_MachineEntities.end())
	{
		dynamic_cast<ToolNodeEntity*>(entry->second)->NotifyToolsChanged();
	}
}

void CNCSimMachineEntity::OnGeometriesChanged(
	cncsim::mwEngine& /*sender*/, cncsim::mwMachineGeometryNode node)
{
	if (!m_Initialized)
		return;
std::cout << "OnGeometriesChanged" << std::endl;
	std::lock_guard<std::mutex> entitiesAccessLock(m_EntitiesAccessMutex);
	typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
	EntityEntry entry = m_MachineEntities.find(node.GetId());
	if (entry != m_MachineEntities.end())
	{
		dynamic_cast<MachineGeometryNodeEntity*>(entry->second)->NotifyGeometriesChanged();
	}
}

void CNCSimMachineEntity::OnRotatedGeometriesChanged(
	cncsim::mwEngine& /*sender*/, cncsim::mwMachineGeometryNode node)
{
	if (!m_Initialized)
		return;
std::cout << "OnRotatedGeometriesChanged" << std::endl;
	std::lock_guard<std::mutex> entitiesAccessLock(m_EntitiesAccessMutex);
	typedef std::unordered_map<int, GeometryNodeEntity*>::iterator EntityEntry;
	EntityEntry entry = m_MachineEntities.find(node.GetId());
	if (entry != m_MachineEntities.end())
	{
		dynamic_cast<MachineGeometryNodeEntity*>(entry->second)->NotifyRotatedGeometriesChanged();
    }
}

void CNCSimMachineEntity::OnIsInCollisionChanged(cncsim::mwEngine &sender, cncsim::mwGeometryNode node)
{
    if (node.GetIsInCollision())
        node.SetOverrideColor({1.0, 0.0, 0.0});
    else
        node.RemoveOverrideColor();
}


const char *MyShader::s_vertexShaderText = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n" // 顶点纹理坐标
    "out vec4 pos;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   pos = gl_Position;\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";
const char *MyShader::s_fragmentShaderText = "#version 330 core\n"
    "in vec4 pos;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D myTexture;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(myTexture, TexCoord);\n"
    "}\n\0";


MyShader::MyShader() : ShaderProgram(s_vertexShaderText, s_fragmentShaderText)
{
    m_shaderLocations.VertexLocation =
        glGetAttribLocation(m_shaderProgram, "aPos");
    m_shaderLocations.TextureLocation =
        glGetUniformLocation(m_shaderProgram, "myTexture");
    m_shaderLocations.UVLocation =
        glGetAttribLocation(m_shaderProgram, "aTexCoord");
//    m_shaderLocations.NormalLocation =
//        glGetUniformLocation(m_shaderProgram, "normal");
//    m_shaderLocations.MatrixLocation =
//        glGetUniformLocation(m_shaderProgram, "matrix");
}

const MyShader::Locations& MyShader::GetShaderLocations() const
{
    return m_shaderLocations;
}

void MyShader::Activate() const
{
    ShaderProgram::Activate();
    (const_cast<MyShader*>(this))->glEnableVertexAttribArray(m_shaderLocations.VertexLocation);
    (const_cast<MyShader*>(this))->glEnableVertexAttribArray(m_shaderLocations.UVLocation);
}

void MyShader::Deactivate() const
{
    ShaderProgram::Deactivate();
    (const_cast<MyShader*>(this))->glDisableVertexAttribArray(m_shaderLocations.VertexLocation);
    (const_cast<MyShader*>(this))->glDisableVertexAttribArray(m_shaderLocations.UVLocation);
}


}  // namespace mwCNCSimVisualizationSamples
