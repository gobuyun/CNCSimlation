// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "StockNodeEntity.hpp"

#include "mwEngine.hpp"
#include "mwStockTriangulation.hpp"

#include <iostream>

namespace mwCNCSimVisualizationSamples
{
class StockNodeEntity::DrawActionHandler : public cncsim::mwDrawActionHandler
{
public:
	DrawActionHandler(std::shared_ptr<GroupRendererMap> triangleGroupRenderers)
		: m_triangleGroupRenderers(std::move(triangleGroupRenderers))
	{
	}

	// Methods inherited from cncsim::mwDrawActionHandler
	void ExecuteAction(const cncsim::DrawTrianglesAction& action) override;
	void ExecuteAction(const cncsim::DrawLinesAction& action) override;
	void ExecuteAction(const cncsim::DeleteAction& action) override;
	void ExecuteAction(const cncsim::SetGroupVisibilityAction& action) override;

	bool SupportsShortIndices() const override
	{
		// Implements enhanced vertex handling with dynamic index size
		return true;
	}

private:
	std::shared_ptr<GroupRendererMap> m_triangleGroupRenderers;
};

StockNodeEntity::StockNodeEntity(const cncsim::mwStockNode& stockNode)
	: m_Node(stockNode), m_triangleGroupRenderers(std::make_shared<GroupRendererMap>())
{
    initializeOpenGLFunctions();
	m_drawActionCollector = m_Node.RegisterDrawActionHandler(
		std::make_shared<DrawActionHandler>(m_triangleGroupRenderers));
}


cadcam::mw3dBoundingBox<float> StockNodeEntity::GetVisualBoundingBox() const
{
	return m_BoundingBox;
}

void StockNodeEntity::NotifyVisualRepresentationChanged()
{
	// Nothing to do yet
	// The colors are part of the triangles
}

/// [PrepareForDrawing]
void StockNodeEntity::PrepareForDrawing()
{
	m_drawActionCollector->BufferDrawActions();
	m_drawActionCollector->ExecuteBufferedActions();

	m_BoundingBox = m_Node.GetGlobalBoundingBox();
	m_Transformation = m_Node.GetGlobalVisualMatrix();
}
/// [PrepareForDrawing]

/// [Draw]
void StockNodeEntity::Draw(
	const VertexColoredShader::VertexColoredShaderLocations& shaderLocations,
	const cadcam::mwHomogenousMatrix& viewmatrix)
{
    misc::mwColor overrideColor;
    int bEnableOverrideColor = m_Node.GetOverrideColor(overrideColor);
    if (bEnableOverrideColor)
    {
        glUniform3f(shaderLocations.OverrideColor, overrideColor.GetRed(), overrideColor.GetGreen(), overrideColor.GetBlue());
    }
    glUniform1i(shaderLocations.EnableOverrideColor, bEnableOverrideColor);

	// Update matrices
	cadcam::mwHomogenousMatrix mv = viewmatrix * m_Transformation;
	cadcam::mwHomogenousMatrix mvNormal = mv;
	mvNormal.Transpose();
	mvNormal = !mvNormal;

	glUniformMatrix4fv(shaderLocations.MVMatrixLocation, 1, GL_TRUE, (const GLfloat*)mv.GetData());
	glUniformMatrix4fv(
		shaderLocations.NormalMVMatrixLocation, 1, GL_TRUE, (const GLfloat*)mvNormal.GetData());

	// Draw all visible renderers
	for (const auto& kvPair : *m_triangleGroupRenderers)
	{
		if (kvPair.second->GetIsVisible())
		{
			kvPair.second->Draw(shaderLocations);
		}
	}
}
/// [Draw]

/// [ExecuteActionTriangle]
void StockNodeEntity::DrawActionHandler::ExecuteAction(const cncsim::DrawTrianglesAction& action)
{
	m_triangleGroupRenderers->insert_or_assign(
		action.GetGroupId(), std::make_unique<StockTriangleGroupRenderer>(action));
}
/// [ExecuteActionTriangle]

/// [ExecuteActionLines]
void StockNodeEntity::DrawActionHandler::ExecuteAction(const cncsim::DrawLinesAction& /*action*/)
{
	// Line actions have not been implemented and will therefore be ignored.
	// Generally they work similar to triangle actions except for using line lists.
}
/// [ExecuteActionLines]

/// [ExecuteActionDelete]
void StockNodeEntity::DrawActionHandler::ExecuteAction(const cncsim::DeleteAction& action)
{
	// Find the entry and delete if available
	m_triangleGroupRenderers->erase(action.GetGroupId());
}
/// [ExecuteActionDelete]

/// [ExecuteActionVisibility]
void StockNodeEntity::DrawActionHandler::ExecuteAction(
	const cncsim::SetGroupVisibilityAction& action)
{
	/// Find the entry and update its visibility
	auto entry = m_triangleGroupRenderers->find(action.GetGroupId());
	if (entry != m_triangleGroupRenderers->end())
	{
		entry->second->SetIsVisible(action.IsVisible());
	}
}
/// [ExecuteActionVisibility]
}  // namespace mwCNCSimVisualizationSamples
