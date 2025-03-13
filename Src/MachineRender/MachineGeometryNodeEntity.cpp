// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "MachineGeometryNodeEntity.hpp"
#include <QDebug>
#include <mwTransformationNode.hpp>

namespace mwCNCSimVisualizationSamples
{
MachineGeometryNodeEntity::MachineGeometryNodeEntity(const cncsim::mwMachineGeometryNode& node)
	: UnicoloredNodeEntity(),
	  m_Node(node),
	  m_NodeIsSpinning(false),
	  m_VisualRepresentationChanged(true)
{
    initializeOpenGLFunctions();
	// Trigger initial geometry update
	NotifyGeometriesChanged();
	NotifyRotatedGeometriesChanged();
}

MachineGeometryNodeEntity::~MachineGeometryNodeEntity()
{
}

cadcam::mw3dBoundingBox<float> MachineGeometryNodeEntity::GetVisualBoundingBox() const
{
	return m_VisualBoundingBox;
}

/// [PrepareForDrawing]
void MachineGeometryNodeEntity::PrepareForDrawing()
{
	// Update visuals
	CheckAndRecreateRenderers();
	UpdateVisualRepresentation();
	m_NodeIsSpinning = m_Node.IsSpinning();
	UpdatePosition();

	// Prepare renderers
	RendererSet& currentRenderers = m_NodeIsSpinning ? m_RotatedRenderers : m_NonRotatedRenderers;
	for (auto& renderer : currentRenderers)
	{
		renderer->PrepareForDrawing();
	}
}
/// [PrepareForDrawing]

/// [Draw]
void MachineGeometryNodeEntity::Draw(
	const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
	const cadcam::mwHomogenousMatrix& viewMatrix)
{
    misc::mwColor overrideColor;
    int bEnableOverrideColor = m_Node.GetOverrideColor(overrideColor);
    if (bEnableOverrideColor)
    {
        glUniform3f(shaderLocations.OverrideColor, overrideColor.GetRed(), overrideColor.GetGreen(), overrideColor.GetBlue());
    }
    glUniform1i(shaderLocations.EnableOverrideColor, bEnableOverrideColor);

	// Select correct renderers
	RendererSet& currentRenderers = m_NodeIsSpinning ? m_RotatedRenderers : m_NonRotatedRenderers;

	// Draw
	for (auto& renderer : currentRenderers)
	{
		renderer->Draw(shaderLocations, viewMatrix);
	}
}
/// [Draw]

/// [NotifyVisualRepresentationChanged]
void MachineGeometryNodeEntity::NotifyVisualRepresentationChanged()
{
    m_VisualRepresentationChanged = true;
}
/// [NotifyVisualRepresentationChanged]

/// [NotifyGeometriesChanged]
void MachineGeometryNodeEntity::NotifyGeometriesChanged()
{
	auto meshes = std::make_shared<ConstMeshSet>(
		m_Node.GetMeshes(cncsim::MGMU_Visual, cncsim::MGMT_NonRotated));
	std::atomic_store(&m_nonrotatedMeshes, meshes);
}
/// [NotifyGeometriesChanged]

/// [NotifyRotatedGeometriesChanged]
void MachineGeometryNodeEntity::NotifyRotatedGeometriesChanged()
{
	auto meshes =
		std::make_shared<ConstMeshSet>(m_Node.GetMeshes(cncsim::MGMU_Visual, cncsim::MGMT_Rotated));
	std::atomic_store(&m_rotatedMeshes, meshes);
}
/// [NotifyRotatedGeometriesChanged]

/// [CheckAndRecreateRenderers]
void MachineGeometryNodeEntity::CheckAndRecreateRenderers()
{
	if (auto meshes = std::atomic_exchange(&m_rotatedMeshes, std::shared_ptr<ConstMeshSet>()))
	{
		RecreateRenderers(*meshes, m_RotatedRenderers);
	}

	if (auto meshes = std::atomic_exchange(&m_nonrotatedMeshes, std::shared_ptr<ConstMeshSet>()))
	{
		RecreateRenderers(*meshes, m_NonRotatedRenderers);
	}
}

void MachineGeometryNodeEntity::RecreateRenderers(
	const ConstMeshSet& meshes, RendererSet& renderersOut)
{
	renderersOut.clear();

    misc::mwColor nodeColor;
    auto isOveridden = m_Node.GetOverrideColor(nodeColor);
    nodeColor = isOveridden? nodeColor:m_Node.GetDisplayInfo().GetColor();
	renderersOut.reserve(meshes.size());
	for (auto& mesh : meshes)
	{
		renderersOut.push_back(std::make_shared<SimpleMeshRenderer>(mesh, nodeColor));
	}
}
/// [CheckAndRecreateRenderers]

/// [UpdateVisualRepresentation]
void MachineGeometryNodeEntity::UpdateVisualRepresentation()
{
	if (m_VisualRepresentationChanged)
	{
		m_VisualRepresentationChanged = false;
		misc::mwColor nodeColor = m_Node.GetDisplayInfo().GetColor();

		// For now only the color needs to be updated.
		for (auto& renderer : m_RotatedRenderers)
		{
			renderer->SetColor(nodeColor);
		}
		for (auto& renderer : m_NonRotatedRenderers)
		{
			renderer->SetColor(nodeColor);
		}
	}
}
/// [UpdateVisualRepresentation]

/// [UpdatePosition]
void MachineGeometryNodeEntity::UpdatePosition()
{
	// Get current data and correct renderers
	cadcam::mwHomogenousMatrix transformation = m_Node.GetGlobalVisualMatrix();
//    if (m_Node.GetName() == "x_axis")
//    {
//        qDebug() << transformation.ToString().ToUTF8().c_str();
//    }
	RendererSet& currentRenderers = m_NodeIsSpinning ? m_RotatedRenderers : m_NonRotatedRenderers;

	// Perform update
	m_VisualBoundingBox.Uninitialize();
	for (auto& renderer : currentRenderers)
	{
		renderer->SetTransformation(transformation);
		m_VisualBoundingBox += renderer->GetBoundingBox();
	}
}
/// [UpdatePosition]

}  // namespace mwCNCSimVisualizationSamples
