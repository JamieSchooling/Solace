#include "Rendering/MeshRender.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Assets/MaterialAssetCache.h"

void MeshRenderComponent::Initialise()
{
	ReloadMesh();
	ReloadMaterial();
}

void MeshRenderComponent::ReloadMesh()
{
	if (Mesh.is_nil())
	{
		return;
	}
	auto path = AssetRegistry::Get().GetFullPath(Mesh);
	if (path.empty() || !std::filesystem::exists(path))
	{
		return;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(),
	aiProcess_Triangulate |
	aiProcess_GenNormals |
	aiProcess_JoinIdenticalVertices |
	aiProcess_SortByPType);

	std::vector<uint32_t> indices;
	std::vector<float> verts;

	const aiMesh* mesh = scene->mMeshes[0];

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	Geometry = std::make_shared<VAO>(indices);

	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		// Positions
		verts.push_back(mesh->mVertices[i].x);
		verts.push_back(mesh->mVertices[i].y);
		verts.push_back(mesh->mVertices[i].z);

		// Normals
		verts.push_back(mesh->mNormals[i].x);
		verts.push_back(mesh->mNormals[i].y);
		verts.push_back(mesh->mNormals[i].z);

		// UVs
		verts.push_back(mesh->mTextureCoords[0][i].x);
		verts.push_back(mesh->mTextureCoords[0][i].y);
	}

	Geometry->AddVertexBuffer(verts, { { 3, ShaderDataType::Float, false, 0 }, { 3, ShaderDataType::Float, false, 3 * sizeof(float) }, { 2, ShaderDataType::Float, false, 6 * sizeof(float) } });
}

void MeshRenderComponent::ReloadMaterial()
{
	Material = MaterialAssetCache::Load(MaterialAsset);
}