// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "UnicoloredNodeEntity.hpp"
#include "UnicoloredShader.hpp"
#include "VertexColoredNodeEntity.hpp"
#include "VertexColoredShader.hpp"
#include "mw3dBoundingBox.hpp"
#include "mwEngineListener.hpp"
#include "mwWarningPragmas.hpp"

#include <QOpenGLFunctions>

#include <QOpenGLShaderProgram>
#include <functional>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace mwCNCSimVisualizationSamples
{
class GeometryNodeEntity;
class OwnVisualizationSample;

class MyShader : public ShaderProgram
{
public:
    /// Contains locations of uniforms and attributes in the shader.
    class Locations
    {
    public:
        GLint VertexLocation;
        GLint NormalLocation;
        GLint MatrixLocation;
        GLint TextureLocation;
        GLint UVLocation;
    };

private:
    static const char* s_vertexShaderText;
    static const char* s_fragmentShaderText;
public:
    /// Creates a basic shader program.
    ///
    /// Needs to be called with an active rendering context.
    MyShader();

public:
    /// Returns the locations of uniforms and attributes in the shader.
    ///
    /// @return Locations of uniforms and attributes in the shader.
    const Locations& GetShaderLocations() const;

public:
    void Activate() const override;
    void Deactivate() const override;

private:
    Locations m_shaderLocations;
};

MW_WARNING_PUSH
MW_WARNING_DISABLE_DEPRECATED  // implementing deprecated events (no compatibility with geometries
							   // in clamps and jaws yet)

	/// Allows basic drawing of CNCSim machines.
class CNCSimMachineEntity : public cncsim::mwEngineListener, protected QOpenGLFunctions
{
public:
	/// Creates a drawer for a simulation.
	///
	/// Must be created with a valid render context.
	/// Correct order:
	/// - Create CNCSimMachineDrawer
	/// - Register as Listener
	/// - Initialize
	/// @param engine The engine for which the drawer will be registered as a listener.
	CNCSimMachineEntity(const misc::mwAutoPointer<cncsim::mwEngine>& engine);

	/// The drawer needs to be deleted from the UI thread with activated render context.
	~CNCSimMachineEntity() override;

public:
	/// The object needs to be initialized once to get the current state.
	///
	/// Updates will not be processed before initialization.
	void Initialize();

    void RegisterOwnVisualObject(OwnVisualizationSample* sample);

public:
	/// Prepares all entities for drawing.
	///
	/// Updates include position, color etc..
	void PrepareForDrawing();

	/// Draws the machine.
	///
	/// @param projectionMat Projection matrix to be used.
	/// @param viewMat View (LookAt) matrix to be used.
	void Draw(
        const cadcam::mwHomogenousMatrix& projectionMat, const cadcam::mwHomogenousMatrix& viewMat);

public:
	/// Gets the current visual bounding box.
	///
	/// The visual bounding box is updated when calling PrepareForDrawing.
	/// @return The current visual bounding box.
	const cadcam::mw3dBoundingBox<float>& GetVisualBoundingBox() const;

private:
	/// Recursively iterates over a subtree and applies a function to each node.
	///
	/// @param subtreeRoot The root of the subtree over which should be iterated.
	/// @param f The function that should be applied.
	void IterateSubtree(
		const cncsim::mwMachineItemNode& subtreeRoot,
		std::function<void(const cncsim::mwMachineItemNode&)> f);

	/// Recursively creates and adds all entities for a subtree.
	///
	/// Requires m_EntitiesAccessMutex to be locked.
	/// @param subtreeRoot Root of the subtree for which the entities should be created.
	void AddEntitiesForSubtree(const cncsim::mwMachineItemNode& subtreeRoot);

	/// Creates and adds an entity for a machine item node and stores it.
	///
	/// Requires m_EntitiesAccessMutex to be locked.
	/// @param node The node for which the entity should be added.
	void AddEntityForNode(const cncsim::mwMachineItemNode& node);

private:
	/// Recursively removes all entities for a subtree.
	///
	/// Requires m_EntitiesAccessMutex to be locked.
	/// Requires m_DeletableEntitiesMutex to be locked.
	/// @param subtreeRoot Root of the subtree for which the entities should be removed.
	void RemoveEntitiesForSubtree(const cncsim::mwMachineItemNode& subtreeRoot);

	/// Removes an entity for a machine item node and stores them.
	///
	/// Requires m_EntitiesAccesMutex to be locked.
	/// Requires m_DeletableEntitiesMutex to be locked.
	/// @param node The node for which the entity should be removed.
	void RemoveEntityForNode(const cncsim::mwMachineItemNode& node);

	/// Removes all entities that are currently stored.
	void RemoveAllEntities();

	/// Deletes all entities that have been removed.
	///
	/// Needs to be called with a valid render context.
	void DeleteRemovedEntities();

    void drawBackground();
    void readyBgShader();
    void releaseBgResources();

private:
	void OnMachineDefinitionChanged(
		cncsim::mwEngine& sender, cncsim::mwMachineDefinitionNode rootNode) override;
	void OnSubTreeAdded(
		cncsim::mwEngine& sender, cncsim::mwMachineItemNode subTreeRootNode) override;
	void OnSubTreeRemoved(
		cncsim::mwEngine& sender, cncsim::mwMachineItemNode subTreeRootNode) override;
	void OnVisualRepresentationChanged(
		cncsim::mwEngine& sender, cncsim::mwGeometryNode node) override;
	void OnToolsChanged(cncsim::mwEngine& sender, cncsim::mwToolNode node) override;
	void OnGeometriesChanged(cncsim::mwEngine& sender, cncsim::mwMachineGeometryNode node) override;
	void OnRotatedGeometriesChanged(
		cncsim::mwEngine& sender, cncsim::mwMachineGeometryNode node) override;
    virtual void OnIsInCollisionChanged(
            cncsim::mwEngine& sender, cncsim::mwGeometryNode node) override;

private:
	/// The engine for which the drawer was created
	misc::mwAutoPointer<cncsim::mwEngine> m_Engine;

	/// The drawer needs to be initialized before registering as listener or drawing
	bool m_Initialized;

	/// Shader for geometry nodes except for stock nodes
	UnicoloredShader m_unicoloredShader;
	/// Shader for stock nodes.
	VertexColoredShader m_vertexColoredShader;

	std::mutex m_EntitiesAccessMutex;
	std::mutex m_DeletableEntitiesMutex;

	/// A mapping from node id to entity
	std::unordered_map<int, GeometryNodeEntity*> m_MachineEntities;
	/// All entities that can be deleted
	std::vector<GeometryNodeEntity*> m_DeletableEntities;
	/// Entities (except for stocks) that have been prepared for drawing
	std::vector<UnicoloredNodeEntity*> m_PreparedUnicoloredEntities;
	/// Stock entities that have been prepared for drawing
	std::vector<VertexColoredNodeEntity*> m_PreparedVertexColoredEntities;

	/// Boundingbox of all elements in the current position
	cadcam::mw3dBoundingBox<float> m_VisualBoundingBox;

    OwnVisualizationSample *m_sample;

    qreal   m_fAngle;
    qreal   m_fScale;

    unsigned int m_texture = -1;
    GLuint m_posVer;
    std::vector<float> m_vertexData;

    MyShader s;

    unsigned char *m_bgTextureData = nullptr;
};

MW_WARNING_POP
}  // namespace mwCNCSimVisualizationSamples
