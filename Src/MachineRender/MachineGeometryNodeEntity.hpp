// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "SimpleMeshRenderer.hpp"
#include "UnicoloredNodeEntity.hpp"
#include "mwMachineGeometryNode.hpp"

namespace mwCNCSimVisualizationSamples
{
/// An entity for drawing a cncsim::mwMachineGeometryNode.
class MachineGeometryNodeEntity : public UnicoloredNodeEntity, protected QOpenGLFunctions
{
	using ConstMeshSet = cncsim::mwMachineGeometryNode::ConstMeshSet;
	using RendererSet = std::vector<std::shared_ptr<SimpleMeshRenderer>>;

public:
	/// Creates a new entity for a node.
	///
	/// @param node The cncsim::mwMachineGeometryNode for which the entity should be created.
	MachineGeometryNodeEntity(const cncsim::mwMachineGeometryNode& node);

	/// The destructor needs to be called from the render thread with an active rendering context.
	~MachineGeometryNodeEntity() override;

public:
	cadcam::mw3dBoundingBox<float> GetVisualBoundingBox() const override;
	void PrepareForDrawing() override;
	void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix) override;
	void NotifyVisualRepresentationChanged() override;

public:
	/// Invalidates the stored non rotated geometries.
	void NotifyGeometriesChanged();
	/// Invalidates the stored rotated geometries.
	void NotifyRotatedGeometriesChanged();

private:
	/// Checks if meshes have changed and in that case recreates the renderers.
	void CheckAndRecreateRenderers();

	/// Recreates renderers for a set of meshes.
	///
	/// The old renderers will first be deleted, then new ones will be created.
	/// @param meshes The meshes from which the renderers will be created.
	/// @param renderersOut The vector which will first be cleaned and then used for the new
	/// renderers.
	void RecreateRenderers(const ConstMeshSet& meshes, RendererSet& renderersOut);

	/// Updates visual representation of all renderers.
	void UpdateVisualRepresentation();

private:
	/// Updates the transformation matrix for all active renderers and also calculates their
	/// bounding box.
	void UpdatePosition();

private:
	cncsim::mwMachineGeometryNode m_Node;

	bool m_NodeIsSpinning;
	bool m_VisualRepresentationChanged;

	std::shared_ptr<ConstMeshSet> m_nonrotatedMeshes;
	std::shared_ptr<ConstMeshSet> m_rotatedMeshes;
	RendererSet m_RotatedRenderers;
	RendererSet m_NonRotatedRenderers;

	cadcam::mw3dBoundingBox<float> m_VisualBoundingBox;
};
}  // namespace mwCNCSimVisualizationSamples
