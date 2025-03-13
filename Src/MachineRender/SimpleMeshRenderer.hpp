// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once

#include <QOpenGLFunctions>

#include "UnicoloredShader.hpp"
#include "mwColor.hpp"
#include "mwMeshLoader.hpp"


namespace mwCNCSimVisualizationSamples
{
class SimpleMeshRenderer : protected QOpenGLFunctions
{
public:
	/// Creates a simple entity for displaying a mesh.
	///
	/// @param mesh The mesh to be displayed.
	/// @param color The initial color.
	SimpleMeshRenderer(
		const misc::mwAutoPointer<const cadcam::mwfMesh>& mesh, const misc::mwColor& color);

	/// Deletes the renderer and cleans up resources.
	///
	/// @warning The renderer need to be deleted with the valid render context
	/// in which is was used being set.
	~SimpleMeshRenderer();

public:
	/// Sets the color to be used when drawing the entity.
	///
	/// @param color The color of the entity.
	void SetColor(const misc::mwColor& color);

	/// Gets the color to be used when drawing the entity.
	///
	/// @return The color of the entity.
	const misc::mwColor& GetColor() const;

public:
	/// Gets the transformed global bounding box of the entity.
	///
	/// @return The transformed global bounding box of the entity.
	cadcam::mw3dBoundingBox<float> GetBoundingBox() const;

public:
	/// Sets the transformation matrix of the entity.
	///
	/// @param transformation The entities transformation matrix.
	void SetTransformation(const cadcam::mwHomogenousMatrix& transformation);

public:
	/// Prepares the entity for drawing.
	///
	/// Must be called with a valid render context before drawing.
	void PrepareForDrawing();

public:
	/// Draws the node using a standard shader.
	///
	/// @param shaderLocations The locations of the current shader program.
	/// @param viewMatrix The current lookat matrix.
	void Draw(
		const UnicoloredShader::UnicoloredShaderLocations& shaderLocations,
		const cadcam::mwHomogenousMatrix& viewMatrix);

public:
	mwMeshLoader::LoadedMesh m_LoadedMesh;
	misc::mwColor m_Color;
	cadcam::mwHomogenousMatrix m_Transformation;
	misc::mwAutoPointer<const cadcam::mwfMesh> m_MeshToBeLoaded;
	cadcam::mw3dBoundingBox<float> m_InitialBoundingBox;
};
}  // namespace mwCNCSimVisualizationSamples
