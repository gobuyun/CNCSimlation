// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "SimulationToolRenderer.hpp"
#include "UnicoloredNodeEntity.hpp"
#include "UnicoloredShader.hpp"
#include "mwToolNode.hpp"

namespace mwCNCSimVisualizationSamples
{
/// An entity for drawing a cncsim::mwToolNode.
class ToolNodeEntity : public UnicoloredNodeEntity, protected QOpenGLFunctions
{
public:
	/// Creates a new entity for a node.
	///
	/// @param node The cncsim::mwToolNode for which the entity should be created.
	ToolNodeEntity(const cncsim::mwToolNode& node);

	/// The destructor needs to be called from the render thread with an active rendering context.
	~ToolNodeEntity() override;

public:
	cadcam::mw3dBoundingBox<float> GetVisualBoundingBox() const override;
	void PrepareForDrawing() override;
	void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewmatrix) override;

public:
	/// Invalidates the stored tool meshes.
	void NotifyToolsChanged();
	/// Invalidates the visual representation (colors, ...)
	void NotifyVisualRepresentationChanged() override;

private:
	/// Deletes the old tool renderers and creates new ones.
	void CheckAndRecreateRenderers();

private:
	cncsim::mwToolNode m_ToolNode;
	std::vector<SimulationToolRenderer*> m_ToolRenderers;
	bool m_ToolsChanged;
	bool m_VisualRepresentationChanged;
};
}  // namespace mwCNCSimVisualizationSamples
