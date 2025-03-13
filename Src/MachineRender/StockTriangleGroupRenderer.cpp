// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "StockTriangleGroupRenderer.hpp"

//#include "OpenGLIncludes.hpp"

//#include <GL/gl.h>
#include <cassert>

namespace
{
inline GLenum VertexComponentTypeToGLenum(cncsim::VertexDataFormat::ComponentType type)
{
	switch (type)
	{
	case cncsim::VertexDataFormat::CT_OMITTED: return 0;
	case cncsim::VertexDataFormat::CT_FLOAT: return GL_FLOAT;
	case cncsim::VertexDataFormat::CT_SIGNED_BYTE: return GL_BYTE;
	case cncsim::VertexDataFormat::CT_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	default: assert(false); return 0;
	}
}

inline GLenum IndexElementSizeToGLenum(std::size_t size)
{
	switch (size)
	{
	case sizeof(unsigned short): return GL_UNSIGNED_SHORT;
	case sizeof(unsigned int): return GL_UNSIGNED_INT;
	default: assert(false); return 0;
	}
}
}  // namespace

namespace mwCNCSimVisualizationSamples
{
/// [Constructor]
StockTriangleGroupRenderer::StockTriangleGroupRenderer(const cncsim::DrawTrianglesAction& action)
	: m_VertexBufferId(0),
	  m_IndexBufferId(0),
	  m_IndicesSize(0),
	  m_ElementCount(0),
	  m_IsVisible(true),
	  m_vertexFormat(action.GetVertexDataFormat())
{
    initializeOpenGLFunctions();
	// Store index information
	if (action.AreIndicesValid())
	{
		m_IndicesSize = action.GetIndexElementSize();
		m_ElementCount = (GLsizei)action.GetIndexCount();
		glGenBuffers(1, &m_IndexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			action.GetIndexCount() * action.GetIndexElementSize(),
			action.GetIndexData(),
			GL_STATIC_DRAW);
	}
	else
	{
		m_ElementCount = (GLsizei)action.GetVertexCount();
	}

	// Store vertex information
	glGenBuffers(1, &m_VertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
	glBufferData(
		GL_ARRAY_BUFFER,
		action.GetVertexCount() * action.GetElementSize(),
		action.GetVertexData(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/// [Constructor]

/// [Destructor]
StockTriangleGroupRenderer::~StockTriangleGroupRenderer()
{
	if (m_IndicesSize > 0)
	{
		glDeleteBuffers(1, &m_IndexBufferId);
	}
	glDeleteBuffers(1, &m_VertexBufferId);
}
/// [Destructor]

/// [Draw]
void StockTriangleGroupRenderer::Draw(
	const VertexColoredShader::VertexColoredShaderLocations& locations)
{
	// Update shader information
	assert(m_vertexFormat.HasPositionComponent());  // position for triangles is required
	assert(
		m_vertexFormat.GetPositionType()
		== cncsim::VertexDataFormat::ComponentType::CT_FLOAT);  // shader assumes float
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
	glVertexAttribPointer(
		locations.VertexPosLocation,
		3,
		VertexComponentTypeToGLenum(m_vertexFormat.GetPositionType()),
		GL_FALSE,  // position must not be normalizes
		(GLsizei)m_vertexFormat.GetStride(),
		(GLvoid*)m_vertexFormat.GetPositionOffset());

	assert(m_vertexFormat.HasNormalComponent());  // normals for triangles is required
	assert(
		m_vertexFormat.GetNormalType()
		== cncsim::VertexDataFormat::ComponentType::CT_FLOAT);  // shader assumes float
	glVertexAttribPointer(
		locations.VertexNormalLocation,
		3,
		VertexComponentTypeToGLenum(m_vertexFormat.GetNormalType()),
		GL_FALSE,  // data is already normalized
		(GLsizei)m_vertexFormat.GetStride(),
		(GLvoid*)m_vertexFormat.GetNormalOffset());

	assert(m_vertexFormat.HasColorComponent());  // Uniform color not implemented in this sample
	assert(m_vertexFormat.GetColorHasAlpha());  // alpha-blending not implemented in this sample
	assert(
		m_vertexFormat.GetNormalType()
		== cncsim::VertexDataFormat::ComponentType::CT_FLOAT);  // shader assumes float
	assert(m_vertexFormat.GetColorHasAlpha());
	assert(m_vertexFormat.HasColorComponent());
	glVertexAttribPointer(
		locations.VertexColorLocation,
		m_vertexFormat.GetColorHasAlpha() ? 4 : 3,
		VertexComponentTypeToGLenum(m_vertexFormat.GetColorType()),
		GL_FALSE,  // color must not be normalized
		(GLsizei)m_vertexFormat.GetStride(),
		(GLvoid*)m_vertexFormat.GetColorOffset());

	// Draw elements
	if (m_IndicesSize > 0)
	{
		// Use index buffer if needed
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
		glDrawElements(
			GL_TRIANGLES, m_ElementCount, IndexElementSizeToGLenum(m_IndicesSize), nullptr);
		// Don't forget removing the index buffer.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, m_ElementCount);
	}

	// Remove the vertex buffer from the context
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/// [Draw]

void StockTriangleGroupRenderer::SetIsVisible(bool isVisible)
{
	m_IsVisible = isVisible;
}

bool StockTriangleGroupRenderer::GetIsVisible() const
{
	return m_IsVisible;
}
}  // namespace mwCNCSimVisualizationSamples
