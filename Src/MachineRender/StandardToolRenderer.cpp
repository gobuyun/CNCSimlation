// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#ifndef SimulationToolRenderer_cpp__
#define SimulationToolRenderer_cpp__

#include "StandardToolRenderer.hpp"

#include <stdexcept>

namespace mwCNCSimVisualizationSamples
{
StandardToolRenderer::StandardToolRenderer(
	const cncsim::mwConstSimulationToolPtr& simulationTool, const cncsim::mwToolNode& toolNode)
	: SimulationToolRenderer(simulationTool, toolNode), m_RendererSet({{nullptr}})
{
	if (simulationTool->GetUsageType() == cncsim::mwSimulationTool::UT_WIRE)
	{
		throw std::runtime_error("This renderer should not be used for wire tools.");
	}

	CreateRenderers();
}

StandardToolRenderer::~StandardToolRenderer()
{
	// Delete all renderers, which also frees the OpenGL resources.
	for (int i = 0; i < 4; ++i)
	{
		if (m_RendererSet.RendererArray[i])
		{
			delete m_RendererSet.RendererArray[i];
			m_RendererSet.RendererArray[i] = nullptr;
		}
	}
}

cadcam::mw3dBoundingBox<float> StandardToolRenderer::GetBoundingBox() const
{
	cadcam::mw3dBoundingBox<float> bb(m_InitialBoundingBox);
	bb.Transform(m_Transformation);
	return bb;
}

void StandardToolRenderer::UpdateDisplayInfo(const cncsim::mwIToolDisplayInfo& /*displayInfo*/)
{
	m_DisplayInfoChanged = true;
}

void StandardToolRenderer::PrepareForDrawing()
{
	// Update positions
	m_Transformation = GetToolNode().GetGlobalVisualMatrix();
	for (int i = 0; i < 4; ++i)
	{
		if (m_RendererSet.RendererArray[i])
		{
			m_RendererSet.RendererArray[i]->SetTransformation(m_Transformation);
		}
	}

	// Update colors
	UpdateVisualRepresentation();

	// Prepare renderers
	for (int i = 0; i < 4; ++i)
	{
		if (m_RendererSet.RendererArray[i])
		{
			m_RendererSet.RendererArray[i]->PrepareForDrawing();
		}
	}
}

void StandardToolRenderer::Draw(
	const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
	const cadcam::mwHomogenousMatrix& viewMatrix)
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_RendererSet.RendererArray[i])
		{
			m_RendererSet.RendererArray[i]->Draw(shaderLocations, viewMatrix);
		}
	}
}

/// [CreateRenderers]
void StandardToolRenderer::CreateRenderers()
{
	// Get the needed information
	const cncsim::mwSimulationTool& tool = *GetSimulationTool();
	const cncsim::mwIToolDisplayInfo& displayInfo = GetToolNode().GetDisplayInfo();

	// We use the current display info, so no further update is needed.
	m_DisplayInfoChanged = false;

	// Create the serveral renderers
	if (tool.GetHolderMesh() != nullptr)
	{
		m_RendererSet.RendererSet.HolderRenderer = new SimpleMeshRenderer(
			tool.GetHolderMesh(), displayInfo.GetHolderColorInfo().GetColor());
	}
	if (tool.GetArborMesh() != nullptr)
	{
		m_RendererSet.RendererSet.ArborRenderer =
			new SimpleMeshRenderer(tool.GetArborMesh(), displayInfo.GetArborColorInfo().GetColor());
	}
	if (tool.GetNonCuttingPartMesh() != nullptr)
	{
		m_RendererSet.RendererSet.ShaftRenderer = new SimpleMeshRenderer(
			tool.GetNonCuttingPartMesh(), displayInfo.GetShaftColorInfo().GetColor());
	}
	if (tool.GetCuttingPartMesh() != nullptr)
	{
		m_RendererSet.RendererSet.FluteRenderer = new SimpleMeshRenderer(
			tool.GetCuttingPartMesh(), displayInfo.GetFluteColorInfo().GetColor());
	}

	// New Renderers also mean a new bounding box.
	m_InitialBoundingBox.Uninitialize();
	for (int i = 0; i < 4; ++i)
	{
		if (m_RendererSet.RendererArray[i])
		{
			m_InitialBoundingBox += m_RendererSet.RendererArray[i]->GetBoundingBox();
		}
	}
}
/// [CreateRenderers]

void StandardToolRenderer::UpdateVisualRepresentation()
{
	if (m_DisplayInfoChanged)
	{
		const cncsim::mwIToolDisplayInfo& displayInfo = GetToolNode().GetDisplayInfo();

		m_DisplayInfoChanged = false;
		if (m_RendererSet.RendererSet.HolderRenderer)
		{
			m_RendererSet.RendererSet.HolderRenderer->SetColor(
				displayInfo.GetHolderColorInfo().GetColor());
		}
		if (m_RendererSet.RendererSet.ArborRenderer)
		{
			m_RendererSet.RendererSet.ArborRenderer->SetColor(
				displayInfo.GetArborColorInfo().GetColor());
		}
		if (m_RendererSet.RendererSet.ShaftRenderer)
		{
			m_RendererSet.RendererSet.ShaftRenderer->SetColor(
				displayInfo.GetShaftColorInfo().GetColor());
		}
		if (m_RendererSet.RendererSet.FluteRenderer)
		{
			m_RendererSet.RendererSet.FluteRenderer->SetColor(
				displayInfo.GetFluteColorInfo().GetColor());
		}
	}
}
}  // namespace mwCNCSimVisualizationSamples

#endif  // SimulationToolRenderer_cpp__
