// (C) 2016 ModuleWorks GmbH
// Owner: CNCSim

#include "mwMeshLoader.hpp"

#include "mwstd/optional.hpp"

#include <cstdio>
#include <memory>
#include <iostream>

namespace mwCNCSimVisualizationSamples
{

QOpenGLFunctions mwMeshLoader::s_openglFn;

mwMeshLoader::LoadedMesh::LoadedMesh()
	: TriangleCount(0), VertexBuffer(0), IndexBuffer(0), UseIndices(false)
{
    mwMeshLoader::s_openglFn.initializeOpenGLFunctions();
}

mwMeshLoader::LoadedMesh mwMeshLoader::LoadMesh(const cadcam::mwfMesh& mesh)
{
	std::vector<float> vertexData;
	std::vector<unsigned int> indexData;

	bool smoothMesh = true;
	float smoothingAngle = 20.f;

    auto openglObj = mwMeshLoader::s_openglFn;

	// Try smoothing mesh if possible
	mwstd::optional<cadcam::mwTMeshVNorm<float>> pMeshVNorm;
	if (smoothMesh)
	{
		try
		{
			pMeshVNorm.emplace(mesh, smoothingAngle);
		}
		catch (const std::bad_alloc&)
		{
			// Mesh smoothing failed due to std::bad_alloc
			// not enough memory to create mwMeshVNorm
			// => fall back to non-smoothed mesh
		}
	};

	// triangles creation
	if (pMeshVNorm.has_value())
	{
		CreateTriangleAndIndexData(*pMeshVNorm, vertexData, indexData);
	}
	else
	{
		CreateTriangleAndIndexData(mesh, vertexData, indexData);
	}

//    vertexData.clear();
//    indexData.clear();
//    vertexData.assign({
//                        -0.9, -0.9, 0, 1.0, 1.0, 1.0,
//                        0.9, -0.9, 0, 1.0, 1.0, 1.0,
//                        -0.9, 0.9, 0, 1.0, 1.0, 1.0
//                    });

	// Upload data
	GLuint vertexBuffer = (unsigned)-1;
	GLuint indexBuffer = (unsigned)-1;
	if (!vertexData.empty())
	{
        openglObj.glGenBuffers(1, &vertexBuffer);
        openglObj.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        openglObj.glBufferData(
			GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
	}
	if (!indexData.empty())
	{
        openglObj.glGenBuffers(1, &indexBuffer);
        openglObj.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        openglObj.glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indexData.size() * sizeof(unsigned int),
			indexData.data(),
			GL_STATIC_DRAW);
	}

	// Generate information
	LoadedMesh loadedMesh;
	loadedMesh.VertexBuffer = vertexBuffer;
	loadedMesh.IndexBuffer = indexBuffer;
	loadedMesh.TriangleCount =
		pMeshVNorm.has_value() ? pMeshVNorm->GetNumberOfTriangles() : mesh.GetNumberOfTriangles();
//    loadedMesh.TriangleCount = 1;
	loadedMesh.UseIndices = !indexData.empty();

//    std::string allVertex;
//    for (auto it : vertexData)
//        {
//            allVertex += std::to_string(it) + "  ";
//        }
//        std::cout << "=========>" << allVertex << std::endl;

    std::cout << "Loaded. [VBO,IBO,Triangles]" << vertexData.size() << "," << indexData.size() << "," << (int)loadedMesh.TriangleCount << std::endl;
	return loadedMesh;
}

void mwMeshLoader::CreateTriangleAndIndexData(
	const cadcam::mwTMeshVNorm<float>& mesh,
	std::vector<float>& vertexDataOut,
	std::vector<unsigned int>& indexDataOut)
{
	vertexDataOut.reserve(
		mesh.GetNumberOfPoints() * 18);  // 3 coordinates for 3 positions + 3 normals
	indexDataOut.reserve(mesh.GetNumberOfTriangles() * 3);  // 3 indices per triangle

	// Copy vertices and normals to one vector
	const auto& normals = mesh.GetNormals();
	auto iterNormals = normals.begin();
	const auto iterNormalsEnd = normals.end();

	const auto& points = mesh.GetPoints();
	auto iterPoints = points.begin();
	const auto iterPointsEnd = points.end();
	for (; iterPoints != iterPointsEnd && iterNormals != iterNormalsEnd;
		 ++iterPoints, ++iterNormals)
	{
		const auto& aPoint = (*iterPoints);
		vertexDataOut.push_back(aPoint.x());
		vertexDataOut.push_back(aPoint.y());
		vertexDataOut.push_back(aPoint.z());

		const auto& aNormal = (*iterNormals);
		vertexDataOut.push_back(aNormal.x());
		vertexDataOut.push_back(aNormal.y());
		vertexDataOut.push_back(aNormal.z());
	}

	// Copy indices to the other
	for (const auto& aTriangle : mesh.GetTriangles())
	{
		indexDataOut.push_back((unsigned int)aTriangle.GetFirstPointIndex());
		indexDataOut.push_back((unsigned int)aTriangle.GetSecondPointIndex());
		indexDataOut.push_back((unsigned int)aTriangle.GetThirdPointIndex());
	}
}

void mwMeshLoader::CreateTriangleAndIndexData(
	const cadcam::mwfMesh& mesh,
	std::vector<float>& vertexDataOut,
	std::vector<unsigned int>& /*indexDataOut*/)
{
	// indices will not be created in this case

	vertexDataOut.reserve(
		mesh.GetNumberOfTriangles() * 18);  // 3 coordinates for 3 positions + 3 normals

	for (size_t i = 0; i < mesh.GetNumberOfTriangles(); i++)
	{
		const cadcam::mwfMesh::point3d& p1 = mesh.GetTriangleFirstVertexPosition(i);

		cadcam::mwfMesh::point3d n = mesh.GetTriangleNormalVector(i);
		n.Normalize();

		vertexDataOut.push_back(p1.x());
		vertexDataOut.push_back(p1.y());
		vertexDataOut.push_back(p1.z());
		vertexDataOut.push_back(n.x());
		vertexDataOut.push_back(n.y());
		vertexDataOut.push_back(n.z());
	}
}

}  // namespace mwCNCSimVisualizationSamples
