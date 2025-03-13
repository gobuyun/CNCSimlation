// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "WireToolRenderer.hpp"

#include "mwMeshFactory.hpp"
#include "mwWireTool.hpp"

#include <cstdio>

namespace mwCNCSimVisualizationSamples
{
WireToolRenderer::WireToolRenderer(
	const cncsim::mwConstSimulationToolPtr& tool, const cncsim::mwWireToolNode& toolNode)
	: SimulationToolRenderer(tool, toolNode),
	  m_WireSideRenderer(nullptr),
	  m_WireBottomRenderer(nullptr),
	  m_WireTopRenderer(nullptr),
	  m_DisplayInfoChanged(false),
	  m_WireToolNode(toolNode)
{
	CreateSideRenderer();
	CreateEndsRenderers();
}

WireToolRenderer::~WireToolRenderer()
{
	if (m_WireSideRenderer)
	{
		delete m_WireSideRenderer;
		m_WireSideRenderer = nullptr;
	}
	if (m_WireBottomRenderer)
	{
		delete m_WireBottomRenderer;
		m_WireBottomRenderer = nullptr;
	}
	if (m_WireTopRenderer)
	{
		delete m_WireTopRenderer;
		m_WireTopRenderer = nullptr;
	}
}

cadcam::mw3dBoundingBox<float> WireToolRenderer::GetBoundingBox() const
{
	cadcam::mw3dBoundingBox<float> bb;
	bb += m_WireSideRenderer->GetBoundingBox();
	bb += m_WireBottomRenderer->GetBoundingBox();
	bb += m_WireTopRenderer->GetBoundingBox();
	return bb;
}

void WireToolRenderer::UpdateDisplayInfo(const cncsim::mwIToolDisplayInfo& /*displayInfo*/)
{
	m_DisplayInfoChanged = true;
}

void WireToolRenderer::PrepareForDrawing()
{
	// Set transformations of renderers
	UpdateTransformations();

	// Set color for wire parts
	if (m_DisplayInfoChanged)
	{
		m_DisplayInfoChanged = false;
		misc::mwColor fluteColor = GetToolNode().GetDisplayInfo().GetFluteColorInfo().GetColor();
		m_WireSideRenderer->SetColor(fluteColor);
		m_WireBottomRenderer->SetColor(fluteColor);
		m_WireTopRenderer->SetColor(fluteColor);
	}

	// Prepare renderers
	m_WireSideRenderer->PrepareForDrawing();
	m_WireBottomRenderer->PrepareForDrawing();
	m_WireTopRenderer->PrepareForDrawing();
}

void WireToolRenderer::Draw(
	const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
	const cadcam::mwHomogenousMatrix& viewMatrix)
{
	m_WireSideRenderer->Draw(shaderLocations, viewMatrix);
	m_WireBottomRenderer->Draw(shaderLocations, viewMatrix);
	m_WireTopRenderer->Draw(shaderLocations, viewMatrix);
}

/// [CreateSideRenderer]
void WireToolRenderer::CreateSideRenderer()
{
	// Create a cylinder hull:
	// Diameter = wire diameter
	// Height = 1, scaling will be performed using transformation matrices
	// Orientation = ZAxis
	const cadcam::mwWireTool& wireTool =
		static_cast<const cadcam::mwWireTool&>(*(GetSimulationTool()->GetTool()));
	float wireRadius = static_cast<float>(wireTool.GetWireDiameter() / 2.);

	cadcam::mw2dPolyLine<float> toolContour;
	toolContour.AddPoint(0.f, wireRadius);
	toolContour.AddPoint(1.f, wireRadius);

	// Creates a mesh from a rotated contour
	cadcam::mwMeshFactory<float>::MeshPtr wireCylinderMesh =
		cadcam::mwMeshFactory<float>::CreateRevolved(
			wireTool.GetUnits(),
			toolContour,
			18.f,
			false,
			cadcam::mwMeshFactory<float>::HAS_TOP_NONE,
			0.f,
			0.f,
			cadcam::mwMeasurableAxis<float>::AXIS_Z);
	cadcam::mwfMeshPtr cadcamMesh = new cadcam::mwfMesh(*wireCylinderMesh);

	// Create the renderer
	misc::mwColor fluteColor = GetToolNode().GetDisplayInfo().GetFluteColorInfo().GetColor();
	m_WireSideRenderer = new SimpleMeshRenderer(cadcamMesh, fluteColor);
}
/// [CreateSideRenderer]

/// [CreateEndsRenderers]
void WireToolRenderer::CreateEndsRenderers()
{
	// Create a disc mesh
	// Diameter = wireDiameter
	// Normal = ZAxis
	const cadcam::mwWireTool& wireTool =
		static_cast<const cadcam::mwWireTool&>(*(GetSimulationTool()->GetTool()));
	float wireRadius = static_cast<float>(wireTool.GetWireDiameter() / 2.);

	cadcam::mwMeshFactory<float>::MeshPtr discMesh =
		cadcam::mwMeshFactory<float>::CreateDisc(wireTool.GetUnits(), wireRadius, 20);
	cadcam::mwfMeshPtr cadcamMesh = new cadcam::mwfMesh(*discMesh);

	// Create two renderers (top and bottom)
	misc::mwColor fluteColor = GetToolNode().GetDisplayInfo().GetFluteColorInfo().GetColor();
	m_WireTopRenderer = new SimpleMeshRenderer(cadcamMesh, fluteColor);
	m_WireBottomRenderer = new SimpleMeshRenderer(cadcamMesh, fluteColor);
}
/// [CreateEndsRenderers]

/// [UpdateTransformations]
void WireToolRenderer::UpdateTransformations()
{
	// Get the current wire length
	float wireLength = m_WireToolNode.GetVisualWireLength();
	if (!wireLength)
	{
		// wire length = 0 leads to matrix problems, so we
		// set it to a small value
		wireLength = 0.1f;
	}

	// Get the basic position/rotation transformation
	cadcam::mwHomogenousMatrix nodeTransformation = GetToolNode().GetGlobalVisualMatrix();

	// Adjust the top disc translation for the tool length
	// Afterwards apply the position transformation
	cadcam::mwHomogenousMatrix topTransformationMatrix;  // identity
	topTransformationMatrix[2][3] = wireLength;  // translation
	topTransformationMatrix = nodeTransformation * topTransformationMatrix;
	m_WireTopRenderer->SetTransformation(topTransformationMatrix);

	// No adjustment needed for the bottom disc.
	m_WireBottomRenderer->SetTransformation(nodeTransformation);

	// Scale the cylinder side to the wire length
	// Afterwards apply the position transformation
	cadcam::mwHomogenousMatrix sideTransformationMatrix;  // identity
	sideTransformationMatrix[2][2] = wireLength;  // scaling by factor of wire length
	sideTransformationMatrix = nodeTransformation * sideTransformationMatrix;
	m_WireSideRenderer->SetTransformation(sideTransformationMatrix);
}
/// [UpdateTransformations]
}  // namespace mwCNCSimVisualizationSamples