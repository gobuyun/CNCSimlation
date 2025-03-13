// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "SimpleMeshRenderer.hpp"

namespace mwCNCSimVisualizationSamples
{
SimpleMeshRenderer::SimpleMeshRenderer(
	const misc::mwAutoPointer<const cadcam::mwfMesh>& mesh, const misc::mwColor& color)
	: m_MeshToBeLoaded(mesh), m_Color(color), m_InitialBoundingBox(mesh->GetBoundingBox())
{
    initializeOpenGLFunctions();
}

SimpleMeshRenderer::~SimpleMeshRenderer()
{
	// Nothing todo yet. Loaded mesh is deleted implicitly
}

void SimpleMeshRenderer::SetColor(const misc::mwColor& color)
{
	m_Color = color;
}

const misc::mwColor& SimpleMeshRenderer::GetColor() const
{
	return m_Color;
}

cadcam::mw3dBoundingBox<float> SimpleMeshRenderer::GetBoundingBox() const
{
	cadcam::mw3dBoundingBox<float> bb(m_InitialBoundingBox);
	bb.Transform(m_Transformation);
	return bb;
}

void SimpleMeshRenderer::SetTransformation(const cadcam::mwHomogenousMatrix& transformation)
{
	m_Transformation = transformation;
}

void SimpleMeshRenderer::PrepareForDrawing()
{
	if (m_MeshToBeLoaded != nullptr)
	{
		m_LoadedMesh = mwMeshLoader::LoadMesh(*m_MeshToBeLoaded);
		m_MeshToBeLoaded = nullptr;
	}
}

void SimpleMeshRenderer::Draw(
	const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
	const cadcam::mwHomogenousMatrix& viewMatrix)
{
	if (m_MeshToBeLoaded != nullptr)
	{
		throw misc::mwException(0, "The entity has not been prepared.");
	}

	cadcam::mwHomogenousMatrix mv = viewMatrix * m_Transformation;
	cadcam::mwHomogenousMatrix mvNormal = mv;
	mvNormal.Transpose();
	mvNormal = !mvNormal;

    glUniformMatrix4fv(shaderLocations.MVMatrixLocation, 1, GL_TRUE, (const GLfloat*)mv.GetData());
    glUniformMatrix4fv(
        shaderLocations.NormalMVMatrixLocation, 1, GL_TRUE, (const GLfloat*)mvNormal.GetData());
    glUniform3fv(shaderLocations.EntityColorLocation, 1, m_Color.GetRGB());

	glBindBuffer(GL_ARRAY_BUFFER, m_LoadedMesh.VertexBuffer);
	glVertexAttribPointer(
		shaderLocations.VertexPosLocation,
		3,
		GL_FLOAT,
		GL_FALSE,
        sizeof(float) * 6,
		(void*)nullptr);
    glVertexAttribPointer(
        shaderLocations.VertexNormalLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 6,
        (void*)(sizeof(float) * 3));

    if (m_LoadedMesh.UseIndices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LoadedMesh.IndexBuffer);
        // Draw front and back.
        glDrawElements(
            GL_TRIANGLES, (GLsizei)m_LoadedMesh.TriangleCount * 3, GL_UNSIGNED_INT, (void*)nullptr);
        glFrontFace(GL_CW);
        glUniform1f(shaderLocations.NormalScale, -1.f);
//        glDrawElements(
//            GL_TRIANGLES, (GLsizei)m_LoadedMesh.TriangleCount * 3, GL_UNSIGNED_INT, (void*)nullptr);
//        glFrontFace(GL_CCW);
//        glUniform1f(shaderLocations.NormalScale, 1.f);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_LoadedMesh.TriangleCount * 3);
        glFrontFace(GL_CW);
        glUniform1f(shaderLocations.NormalScale, -1.f);
//        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_LoadedMesh.TriangleCount * 3);
//        glFrontFace(GL_CCW);
//        glUniform1f(shaderLocations.NormalScale, 1.f);
    }
}
}  // namespace mwCNCSimVisualizationSamples
