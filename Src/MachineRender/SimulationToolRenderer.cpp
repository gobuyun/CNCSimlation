// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "SimulationToolRenderer.hpp"

#include <stdexcept>

namespace mwCNCSimVisualizationSamples
{
SimulationToolRenderer::SimulationToolRenderer(
	const cncsim::mwConstSimulationToolPtr& simulationTool, const cncsim::mwToolNode& toolNode)
	: m_SimulationTool(simulationTool), m_ToolNode(toolNode)
{
	if (simulationTool == nullptr)
	{
		throw std::runtime_error("Cannot create SimulationToolRenderer for null pointer.");
	}
}

SimulationToolRenderer::~SimulationToolRenderer()
{
	// Nothing to do here
}

const cncsim::mwConstSimulationToolPtr& SimulationToolRenderer::GetSimulationTool() const
{
	return m_SimulationTool;
}

const cncsim::mwToolNode& SimulationToolRenderer::GetToolNode() const
{
	return m_ToolNode;
}
}  // namespace mwCNCSimVisualizationSamples