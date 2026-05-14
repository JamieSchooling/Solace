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
		if (outPath && meshProp)
		{
			meshProp->Set(std::string(outPath), r, e);
			m_component->Initialise(r, e);
		}
	}

}
