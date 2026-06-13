#include "Inspectors/MeshRenderInspector.h"
#include "Inspectors/EditorProperty.h"
#include <nfd.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Core/Application.h>

void MeshRenderInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	auto meshProp = m_component->GetProperty("MeshAsset");
	if (meshProp)
	{
		AssetHandle meshHandle = std::any_cast<AssetHandle>(meshProp->Get(r, e));
		if (EditorProperty<AssetHandle>("Mesh", meshHandle).Draw())
		{
			meshProp->Set(meshHandle, r, e);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Item"))
			{
				AssetHandle handle = *(AssetHandle*)payload->Data;
				std::filesystem::path path = AssetRegistry::Get().GetFullPath(handle);
				Assimp::Importer importer;
				if (importer.IsExtensionSupported(path.extension().string().c_str()))
				{
					meshProp->Set(handle, r, e);
					m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMesh();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	auto materialProp = m_component->GetProperty("MaterialAsset");
	if (materialProp)
	{
		AssetHandle materialHandle = std::any_cast<AssetHandle>(materialProp->Get(r, e));
		if (EditorProperty<AssetHandle>("material", materialHandle).Draw())
		{
			materialProp->Set(materialHandle, r, e);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Item"))
			{
				AssetHandle handle = *(AssetHandle*)payload->Data;
				std::filesystem::path path = AssetRegistry::Get().GetFullPath(handle);
				if (path.extension() == ".mat")
				{
					materialProp->Set(handle, r, e);
					m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMaterial();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}