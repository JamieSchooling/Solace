#include "Inspectors/MeshRenderInspector.h"
#include <nfd.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void MeshRenderInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	auto meshProp = m_component->GetProperty("Mesh");
	if (meshProp)
	{
		std::string meshPath = std::any_cast<std::string>(meshProp->Get(r, e));
		if (EditorProperty<std::string>("Mesh", meshPath).Draw(true))
		{
			meshProp->Set(meshPath, r, e);
		}
	}

	if (ImGui::Button("..."))
	{
		nfdchar_t* outPath = nullptr;
		NFD_OpenDialog("fbx;obj;glTF;dae", nullptr, &outPath);
		if (outPath)
		{
			meshProp->Set(std::string(outPath), r, e);
			auto* target = m_component->GetTarget<MeshRenderComponent>(r, e);
			std::cout << outPath << std::endl;

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(outPath,
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
			target->Geometry = std::make_shared<VAO>(indices);

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
			
			target->Geometry->AddVertexBuffer(verts, { { 3, ShaderDataType::Float, false, 0 }, { 3, ShaderDataType::Float, false, 3 * sizeof(float) } });
		}
	}

}
