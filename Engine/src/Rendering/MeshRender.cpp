#include "Rendering/MeshRender.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void MeshRenderComponent::Initialise()
{
	std::cout << "Mesh Initialise: " << Mesh << std::endl;

	std::shared_ptr<Shader> shader = std::make_shared<Shader>("./resources/shaders/Vertex.glsl", "./resources/shaders/Fragment.glsl");
	Material = std::make_shared<::Material>(shader);
	Material->SetValue("u_Colour", glm::vec3(1.0));

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(Mesh,
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
	}

	Geometry->AddVertexBuffer(verts, { { 3, ShaderDataType::Float, false, 0 }, { 3, ShaderDataType::Float, false, 3 * sizeof(float) } });
}