#include "MeshLoader.h"

#pragma warning(push, 0) 
/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)

#include <iostream>
#include <cassert>

using namespace std;

static int toInt(const size_t in) { return static_cast<int>(in); }
static int toInt(const unsigned int in) { return static_cast<int>(in); }
class AISceneHolder final
{
public:
	AISceneHolder(const aiScene* scenePtr) : m_scenePtr(scenePtr) {}
	~AISceneHolder() { aiReleaseImport(m_scenePtr); }

	operator bool() const { return m_scenePtr; }
	const aiScene& operator*() const { return *m_scenePtr; }
	const aiScene* m_scenePtr;
};

string nameIfHas(const bool has, const string& name)
{
	return has ? name : "";
}
unique_ptr<Mesh> MeshLoader::loadMesh(const string& fileName)
{
	unique_ptr<Mesh> result;

	const auto aiStdStream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, nullptr);
	aiAttachLogStream(&aiStdStream);
//	const auto aiErrStream = aiGetPredefinedLogStream(aiDefaultLogStream_STDERR, nullptr);
//	aiAttachLogStream(&aiErrStream);

	//aiProcessPreset_TargetRealtime_MaxQuality
	const auto scenePtr = aiImportFile(fileName.c_str(), 
		  aiProcess_Triangulate 
		| aiProcess_SortByPType 
		| aiProcess_GenSmoothNormals
		| aiProcess_ValidateDataStructure
	    | aiProcess_GenUVCoords);
	if (scenePtr)
	{
		const auto& scene = *scenePtr;
		const int numMeshes = toInt(scene.mNumMeshes);
		if (numMeshes > 0)
		{
			cout << " Meshes:" << endl;
			for (int mi = 0; mi < numMeshes; ++mi)
			{
				const auto& aiMesh = *scene.mMeshes[mi];
				const auto primitiveFlags = aiMesh.mPrimitiveTypes;
				const bool hasPoints    = primitiveFlags & aiPrimitiveType_POINT;
				const bool hasLines     = primitiveFlags & aiPrimitiveType_LINE;
				const bool hasTriangles = primitiveFlags & aiPrimitiveType_TRIANGLE;

				const int numVertices = toInt(aiMesh.mNumVertices);
				const int numFaces = toInt(aiMesh.mNumFaces);

				const bool useMesh = !result && numFaces > 0 && hasTriangles;
				cout << "  " << mi << " : "
					<< " #v= " << numVertices
					<< " #f= " << numFaces
					<< " primitives:"
					<< nameIfHas(hasPoints, " points")
					<< nameIfHas(hasLines, " lines")
					<< nameIfHas(hasTriangles, " triangles")
					<< endl
					;
				if (useMesh)
				{
					result.reset(new Mesh());
					auto& mesh = *result;
					{
						const auto& aiVertices = aiMesh.mVertices;
						auto& vertices = mesh.m_vertices;
						vertices.reserve(numVertices);
						for (int vi = 0; vi < numVertices; ++vi)
							vertices.emplace_back(aiVertices[vi][0], aiVertices[vi][1], aiVertices[vi][2]);
					}
					{
						const auto& aiNormals = aiMesh.mNormals;
						auto& normals = mesh.m_normals;
						normals.reserve(numVertices);
						for (int vi = 0; vi < numVertices; ++vi)
						{
							const auto& aiNormal = aiNormals[vi];
							normals.emplace_back(aiNormal[0], aiNormal[1], aiNormal[2]);
						}
					}
					{
						assert(aiMesh.mNumUVComponents[0] == 2);
						const auto& aiTextureCoords = aiMesh.mTextureCoords[0];
						auto& textureCoords = mesh.m_textureCoords;
						textureCoords.reserve(numVertices);
						for (int vi = 0; vi < numVertices; ++vi)
						{
							const auto& aiTextureCoord = aiTextureCoords[vi];
							textureCoords.emplace_back(aiTextureCoord[0], aiTextureCoord[1]);
						}
					}
					{
						const auto& aiFaces = aiMesh.mFaces;
						auto& faces = mesh.m_faces;
						faces.reserve(numFaces);
						for (int fi = 0; fi < numFaces; ++fi)
						{
							const auto& aiFace = aiFaces[fi];
							assert(aiFace.mNumIndices == 3);
							const auto& aiFVIs = aiFace.mIndices;
							faces.emplace_back(aiFVIs[0], aiFVIs[1], aiFVIs[2]);
						}
					}
				}
			}
		}
		else
		{
			cout << " No meshes found" << endl;
		}

	}

	aiDetachAllLogStreams();

	return result;
}

