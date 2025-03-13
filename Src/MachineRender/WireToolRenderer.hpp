// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "SimpleMeshRenderer.hpp"
#include "SimulationToolRenderer.hpp"
#include "mwSimulationTool.hpp"
#include "mwWireToolNode.hpp"

namespace mwCNCSimVisualizationSamples
{
/// Creates a mesh for a wire tool and renders it.
///
/// The wire will be rendered a s cylinder of dynamic length.
class WireToolRenderer : public SimulationToolRenderer
{
public:
	/// Creates a new renderer for a cncsim::mwSimulationTool.
	///
	/// @param tool The tool for which the renderer should be created.
	/// @param toolNode The wiretool node for which the renderer should be created.
	WireToolRenderer(
		const cncsim::mwConstSimulationToolPtr& tool, const cncsim::mwWireToolNode& toolNode);

	/// Deletes the renderer and cleans up resources.
	///
	/// @warning The renderer need to be deleted with the valid render context
	/// in which is was used being set.
	~WireToolRenderer() override;

public:
	cadcam::mw3dBoundingBox<float> GetBoundingBox() const override;
	void UpdateDisplayInfo(const cncsim::mwIToolDisplayInfo& displayInfo) override;
	void PrepareForDrawing() override;
	void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix) override;

private:
	/// Creates the renderer for the cylinders side
	void CreateSideRenderer();

	/// Creates the renderers for the cylinders ends
	void CreateEndsRenderers();

private:
	/// Updates the transformation matrices of all renderers
	///
	/// This is especially important for the dynamic tool length.
	/// The wire side will be scaled and and one wire end will be
	/// translated additional to the regular position transformation.
	void UpdateTransformations();

private:
	/// The cncsim::mwWireToolNode for which the renderer was created
	cncsim::mwWireToolNode m_WireToolNode;

	/// Renders the cylinders side
	SimpleMeshRenderer* m_WireSideRenderer;
	/// Renders the cylinders top
	SimpleMeshRenderer* m_WireTopRenderer;
	/// Renders the cylinders bottom
	SimpleMeshRenderer* m_WireBottomRenderer;

	/// A flag for notification of changing visuals
	bool m_DisplayInfoChanged;
};
}  // namespace mwCNCSimVisualizationSamples
