// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#pragma once
//#include "OpenGLIncludes.hpp"
//
#include "mwMesh.hpp"
#include "mwMeshVNorm.hpp"
#include <QOpenGLFunctions>
//#include <GL/gl.h>
#include <vector>

namespace mwCNCSimVisualizationSamples
{
/// Utility class for uploading mwMeshes to the graphics card
class mwMeshLoader
{
public:
	/// A mesh loaded to the graphics card
    class LoadedMesh
	{
	public:
		LoadedMesh();

	public:
		/// The amount of triangles in this mesh.
		size_t TriangleCount;
		/// The vertex buffer to which the mesh data has been uploaded.
		GLuint VertexBuffer;
		/// 0 or the index buffer to which the indeces have been uploaded.
		GLuint IndexBuffer;
		/// True if the indexbuffer is filled and should be used, false otherwise.
		bool UseIndices;
	};

public:
	/// Smoothes a mesh and loads it to the graphics card.
	///
	/// @return The mesh loaded to the graphics card.
	static LoadedMesh LoadMesh(const cadcam::mwfMesh& mesh);

private:
	/// Fills vectors with mesh data. The vectors can then be uploaded to the graphics card.
	///
	/// @param mesh The mesh to be uploaded.
	/// @param vertexDataOut Output for vertex data. Format: 3 floats position + 3 floats normal
	/// @param indexDataOut Output for indices. If empty afterwards no indices are required for
	/// drawing.
	static void CreateTriangleAndIndexData(
		const cadcam::mwfMesh& mesh,
		std::vector<float>& vertexDataOut,
		std::vector<unsigned int>& indexDataOut);

	/// Fills vectors with mesh data. The vectors can then be uploaded to the graphics card.
	///
	/// @param mesh The mesh to be uploaded.
	/// @param vertexDataOut Output for vertex data. Format: 3 floats position + 3 floats normal
	/// @param indexDataOut Output for indices. If empty afterwards no indices are required for
	/// drawing.
	static void CreateTriangleAndIndexData(
		const cadcam::mwTMeshVNorm<float>& mesh,
		std::vector<float>& vertexDataOut,
		std::vector<unsigned int>& indexDataOut);

    static QOpenGLFunctions s_openglFn;
};
}  // namespace mwCNCSimVisualizationSamples
