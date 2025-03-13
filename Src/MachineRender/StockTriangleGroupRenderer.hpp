// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
#include "VertexColoredShader.hpp"
//#include "glad/gl.h"
#include "mwStockTriangulation.hpp"
#include <QOpenGLFunctions>
//#include <GL/gl.h>

namespace mwCNCSimVisualizationSamples
{
/// Renders one group of triangles of a stock.
class StockTriangleGroupRenderer : protected QOpenGLFunctions
{
public:
	/// Creates a new renderer for a triangle group.
	///
	/// Must be called from the rendering thread with valid render context.
	/// @param action
	explicit StockTriangleGroupRenderer(const cncsim::DrawTrianglesAction& action);

	/// Deletes the renderer and frees all OpenGL resources.
	///
	/// Must be called from the rendering thread with valid render context.
	~StockTriangleGroupRenderer();

public:
	/// Draws the stock triangle group.
	///
	/// @param locations The shader locations to be used.
	void Draw(const VertexColoredShader::VertexColoredShaderLocations& locations);

public:
	/// Sets whether the group is currently visible
	///
	/// @param isVisible True if the the group should be drawn, false otherwise.
	void SetIsVisible(bool isVisible);

	/// Gets whether the group is currently visible
	///
	/// @return True if the the group should be drawn, false otherwise.
	bool GetIsVisible() const;

private:
	/// Id of the vertex buffer containing vertex positions and normals.
	GLuint m_VertexBufferId;
	/// Id of the index buffer (if any).
	GLuint m_IndexBufferId;
	/// >0 if the index buffer is filled and should be used.
	std::size_t m_IndicesSize;
	/// Amount of vertices to be drawn.
	GLsizei m_ElementCount;
	/// True is the group should be drawn, false otherwise.
	bool m_IsVisible;
	/// Stores the vertex format
	cncsim::VertexDataFormat m_vertexFormat;
};
}  // namespace mwCNCSimVisualizationSamples
