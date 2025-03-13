// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "SimpleMeshRenderer.hpp"
#include "SimulationToolRenderer.hpp"
#include "UnicoloredShader.hpp"
#include "mw3dBoundingBox.hpp"
#include "mwDisplayInfo.hpp"
#include "mwSimulationTool.hpp"
#include "mwToolNode.hpp"

namespace mwCNCSimVisualizationSamples
{
/// Renders a standard cncsim::mwSimulationTool
class StandardToolRenderer : public SimulationToolRenderer
{
public:
	/// Creates a new renderer for a standard cncsim::mwSimulationTool.
	///
	/// @param simulationTool The tool for which the renderer should be created.
	/// @param toolNode The tool node for which the renderer should be created.
	/// @note This renderer should not be used for wire tools.
	StandardToolRenderer(
		const cncsim::mwConstSimulationToolPtr& simulationTool, const cncsim::mwToolNode& toolNode);

	/// Deletes the renderer and cleans up resources.
	///
	/// @warning The renderer need to be deleted with the valid render context
	/// in which is was used being set.
	~StandardToolRenderer() override;

public:
	cadcam::mw3dBoundingBox<float> GetBoundingBox() const override;
	void UpdateDisplayInfo(const cncsim::mwIToolDisplayInfo& displayInfo) override;
	void PrepareForDrawing() override;
	void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix) override;

private:
	/// Creates the renderers for the single tool parts.
	void CreateRenderers();
	/// Updates colors of the all renderers
	void UpdateVisualRepresentation();

private:
	/// [RendererSet]
	/// Contains all required tool part renderers.
	///
	/// The renderers can either be accessed individually
	/// or as an array ordered as {holder, arbor, shaft, flute}.
	/// @note Not all renders need to be available.
	union
	{
		SimpleMeshRenderer* RendererArray[4];
		struct
		{
			SimpleMeshRenderer* HolderRenderer;
			SimpleMeshRenderer* ArborRenderer;
			SimpleMeshRenderer* ShaftRenderer;
			SimpleMeshRenderer* FluteRenderer;
		} RendererSet;
	} m_RendererSet;
	/// [RendererSet]

	cadcam::mw3dBoundingBox<float> m_InitialBoundingBox;
	cadcam::mwHomogenousMatrix m_Transformation;
	bool m_DisplayInfoChanged;
};
}  // namespace mwCNCSimVisualizationSamples
