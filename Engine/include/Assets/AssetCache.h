#pragma once

#include "Rendering/Material.h"
#include <filesystem>

#include "Assets/AssetHandle.h"
#include "Assets/AssetRegistry.h"
#include "Assets/MaterialSerialiser.h"
#include <Rendering/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class AssetCache
{
public:
	template<typename T>
	static std::shared_ptr<T> Load(AssetHandle handle);

	template<typename T>
	static void Delete(AssetHandle handle);
private:
	static inline std::unordered_map<AssetHandle, std::shared_ptr<Material>> m_materialCache;
	static inline std::unordered_map<AssetHandle, std::shared_ptr<Mesh>> m_meshCache;
};

template<typename T>
inline std::shared_ptr<T> AssetCache::Load(AssetHandle handle)
{
	return nullptr;
}

template<typename T>
inline void AssetCache::Delete(AssetHandle handle)
{}

template<>
inline std::shared_ptr<Material> AssetCache::Load(AssetHandle handle)
{
	if (m_materialCache.contains(handle))
	{
		return m_materialCache.at(handle);
	}

	std::shared_ptr<Material> material;
	if (!AssetRegistry::Get().Exists(handle))
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl");
		material = std::make_shared<Material>(shader, MaterialInitFlags_InitWithDefaultValues);
		material->SetValue("u_prop_colour", glm::vec3(1.0));
		material->SetValue("u_prop_specularAmount", 0.5f);
	}
	else
	{
		auto path = AssetRegistry::Get().GetFullPath(handle);
		MaterialSerialiser ms;
		material = ms.DeserialiseFrom(path);
	}

	m_materialCache[handle] = material;
	return material;
}


template<>
inline void AssetCache::Delete<Material>(AssetHandle handle)
{
	m_materialCache.erase(handle);
}

template<>
inline std::shared_ptr<Mesh> AssetCache::Load(AssetHandle handle)
{
	if (m_meshCache.contains(handle))
	{
		return m_meshCache.at(handle);
	}

	if (handle.is_nil())
	{
		return nullptr;
	}
	auto path = AssetRegistry::Get().GetFullPath(handle);
	if (path.empty() || !std::filesystem::exists(path))
	{
		return nullptr;
	}

	std::shared_ptr<Mesh> loadedMesh = std::make_shared<Mesh>();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(),
	aiProcess_Triangulate |
	aiProcess_GenNormals |
	aiProcess_JoinIdenticalVertices |
	aiProcess_SortByPType);

	std::vector<uint32_t> indices;
	std::vector<float> verts;
	std::vector<float> depthVerts;

	const aiMesh* mesh = scene->mMeshes[0];

	for (size_t i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	loadedMesh->Geometry = std::make_shared<VAO>(indices);
	loadedMesh->DepthGeometry = std::make_shared<VAO>(indices);

	for (size_t i = 0; i < mesh->mNumVertices; ++i)
	{
		// Positions
		verts.push_back(mesh->mVertices[i].x);
		verts.push_back(mesh->mVertices[i].y);
		verts.push_back(mesh->mVertices[i].z);

		depthVerts.push_back(mesh->mVertices[i].x);
		depthVerts.push_back(mesh->mVertices[i].y);
		depthVerts.push_back(mesh->mVertices[i].z);

		// Normals
		verts.push_back(mesh->mNormals[i].x);
		verts.push_back(mesh->mNormals[i].y);
		verts.push_back(mesh->mNormals[i].z);

		// UVs
		verts.push_back(mesh->mTextureCoords[0][i].x);
		verts.push_back(mesh->mTextureCoords[0][i].y);
	}

	loadedMesh->Geometry->AddVertexBuffer(verts, { { 3, ShaderDataType::Float, false, 0 }, { 3, ShaderDataType::Float, false, 3 * sizeof(float) }, { 2, ShaderDataType::Float, false, 6 * sizeof(float) } });
	loadedMesh->DepthGeometry->AddVertexBuffer(depthVerts, { { 3, ShaderDataType::Float, false, 0 } });


	m_meshCache[handle] = loadedMesh;
	return loadedMesh;
}


template<>
inline void AssetCache::Delete<Mesh>(AssetHandle handle)
{
	m_meshCache.erase(handle);
}
