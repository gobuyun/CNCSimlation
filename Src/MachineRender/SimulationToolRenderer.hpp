// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "UnicoloredShader.hpp"
#include "mw3dBoundingBox.hpp"
#include "mwDisplayInfo.hpp"
#include "mwSimulationTool.hpp"
#include "mwToolNode.hpp"

namespace mwCNCSimVisualizationSamples
{
/// Renders a cncsim::mwSimulationTool
class SimulationToolRenderer
{
public:
	/// Creates a new renderer for a cncsim::mwSimulationTool.
	///
	/// @param simulationTool The tool for which the renderer should be created.
	/// @param toolNode The tool node for which the renderer should be created.
	SimulationToolRenderer(
		const cncsim::mwConstSimulationToolPtr& simulationTool, const cncsim::mwToolNode& toolNode);

	/// Deletes the renderer and cleans up resources.
	///
	/// @warning The renderer need to be deleted with the valid render context
	/// in which is was used being set.
	virtual ~SimulationToolRenderer();

public:
	/// Gets the transformed global bounding box of the entity.
	///
	/// @return The transformed global bounding box of the entity.
	virtual cadcam::mw3dBoundingBox<float> GetBoundingBox() const = 0;

public:
	/// Updates the renderers visual properties with a cncsim::mwIToolDisplayInfo
	///
	/// @param displayInfo The cncsim::mwIToolDisplayInfo with visual data for this renderer.
	virtual void UpdateDisplayInfo(const cncsim::mwIToolDisplayInfo& displayInfo) = 0;

public:
	/// Prepares the entity for drawing.
	///
	/// Must be called with a valid render context before drawing.
	virtual void PrepareForDrawing() = 0;

public:
	/// Draws the node using a standard shader.
	///
	/// @param shaderLocations The locations of the current shader program.
	/// @param viewMatrix The current lookat matrix.
	virtual void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix) = 0;

public:
	/// Gets the cncsim::mwSimulationTool that is being rendered.
	///
	/// @return An autopointer to the cncsim::mwSimulationTool that is being rendered.
	const cncsim::mwConstSimulationToolPtr& GetSimulationTool() const;

	/// Gets the cncsim:mwToolNode to which the rendered tool belongs.
	///
	/// @return The cncsim:mwToolNode to which the rendered tool belongs.
	const cncsim::mwToolNode& GetToolNode() const;

private:
	cncsim::mwConstSimulationToolPtr m_SimulationTool;
	cncsim::mwToolNode m_ToolNode;
};
}  // namespace mwCNCSimVisualizationSamples
