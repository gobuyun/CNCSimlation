// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "ToolNodeEntity.hpp"

#include "StandardToolRenderer.hpp"
#include "WireToolRenderer.hpp"

namespace mwCNCSimVisualizationSamples
{
ToolNodeEntity::ToolNodeEntity(const cncsim::mwToolNode& node)
	: UnicoloredNodeEntity(),
	  m_ToolNode(node),
	  m_ToolsChanged(true),
	  m_VisualRepresentationChanged(true)
{
    initializeOpenGLFunctions();
}

ToolNodeEntity::~ToolNodeEntity()
{
	for (const SimulationToolRenderer* renderer : m_ToolRenderers)
	{
		delete renderer;
	}
	m_ToolRenderers.clear();
}

cadcam::mw3dBoundingBox<float> ToolNodeEntity::GetVisualBoundingBox() const
{
	cadcam::mw3dBoundingBox<float> bb;
	for (const SimulationToolRenderer* renderer : m_ToolRenderers)
	{
		bb += renderer->GetBoundingBox();
	}
	return bb;
}

/// [PrepareForDrawing]
void ToolNodeEntity::PrepareForDrawing()
{
	// Create new renderers if needed.
	CheckAndRecreateRenderers();

	// Update visual properties of renderers
	if (m_VisualRepresentationChanged)
	{
		m_VisualRepresentationChanged = false;
		const cncsim::mwIToolDisplayInfo& displayInfo = m_ToolNode.GetDisplayInfo();
		for (SimulationToolRenderer* renderer : m_ToolRenderers)
		{
			renderer->UpdateDisplayInfo(displayInfo);
		}
	}

	// Prepare renderers for drawing
	for (SimulationToolRenderer* renderer : m_ToolRenderers)
	{
		// This also loads the meshes to VBOs if not happened yet.
		renderer->PrepareForDrawing();
	}
}
/// [PrepareForDrawing]

/// [Draw]
void ToolNodeEntity::Draw(
	const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
	const cadcam::mwHomogenousMatrix& viewmatrix)
{
    misc::mwColor overrideColor;
    int bEnableOverrideColor = m_ToolNode.GetOverrideColor(overrideColor);
    if (bEnableOverrideColor)
    {
        glUniform3f(shaderLocations.OverrideColor, overrideColor.GetRed(), overrideColor.GetGreen(), overrideColor.GetBlue());
    }
    glUniform1i(shaderLocations.EnableOverrideColor, bEnableOverrideColor);

	for (SimulationToolRenderer* renderer : m_ToolRenderers)
	{
		renderer->Draw(shaderLocations, viewmatrix);
	}
}
/// [Draw]

/// [NotifyToolsChanged]
void ToolNodeEntity::NotifyToolsChanged()
{
	// The renderers will be created lateron when preparing for drawing.
	m_ToolsChanged = true;
}
/// [NotifyToolsChanged]

/// [NotifyVisualRepresentationChanged]
void ToolNodeEntity::NotifyVisualRepresentationChanged()
{
	// The renderers will be updated lateron when preparing for drawing.
	m_VisualRepresentationChanged = true;
}
/// [NotifyVisualRepresentationChanged]

/// [CheckAndRecreateRenderers]
void ToolNodeEntity::CheckAndRecreateRenderers()
{
	if (m_ToolsChanged)
	{
		m_ToolsChanged = false;

		// Delete the old renderers which also frees OpenGL resources.
		for (const SimulationToolRenderer* renderer : m_ToolRenderers)
		{
			delete renderer;
		}
		m_ToolRenderers.clear();

		// Create the new renderers. Meshes will be loaded upon preparing them.
		auto tool = m_ToolNode.GetTool();
		if (tool != nullptr)
		{
			if (auto wireToolNode = m_ToolNode.TryAsNode<cncsim::mwWireToolNode>())
			{
				m_ToolRenderers.push_back(new WireToolRenderer(tool, wireToolNode));
			}
			else
			{
				m_ToolRenderers.push_back(new StandardToolRenderer(tool, m_ToolNode));
			}
		}
	}
}
/// [CheckAndRecreateRenderers]
}  // namespace mwCNCSimVisualizationSamples
