// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "StockTriangleGroupRenderer.hpp"
#include "VertexColoredNodeEntity.hpp"
#include "mwDrawActionCollector.hpp"
#include "mwStockNode.hpp"

#include <unordered_map>

namespace mwCNCSimVisualizationSamples
{
/// An entity for a single stock node.
///
/// The entity will update itself according to cuts on the stock.
class StockNodeEntity : public VertexColoredNodeEntity, protected QOpenGLFunctions
{
public:
	/// Creates a new entity for a stock.
	///
	/// @param stockNode The cncsim::mwStockNode that should be drawn.
	StockNodeEntity(const cncsim::mwStockNode& stockNode);

	/// The destructor needs to be called from the render thread with an active rendering context.
	~StockNodeEntity() override = default;

public:
	cadcam::mw3dBoundingBox<float> GetVisualBoundingBox() const override;
	void NotifyVisualRepresentationChanged() override;
	void PrepareForDrawing() override;
	void Draw(
		const VertexColoredShader::VertexColoredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewmatrix) override;

private:
	cncsim::mwStockNode m_Node;
	cadcam::mw3dBoundingBox<float> m_BoundingBox;
	cadcam::mwHomogenousMatrix m_Transformation;

	class DrawActionHandler;
	using GroupRendererMap = std::unordered_map<int, std::unique_ptr<StockTriangleGroupRenderer>>;
	std::shared_ptr<GroupRendererMap> m_triangleGroupRenderers;
	std::unique_ptr<cncsim::mwDrawActionCollector> m_drawActionCollector;
};
}  // namespace mwCNCSimVisualizationSamples
