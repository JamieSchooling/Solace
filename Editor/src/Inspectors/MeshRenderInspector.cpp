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
		EditResult result = EditorProperty<AssetHandle>("Mesh", meshHandle).Draw();
		if (result.Changed)
		{
			meshProp->Set(meshHandle, r, e);
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginPropertyEdit(meshProp, r, e);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndPropertyEdit(r, e);
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
					UndoSystem::BeginPropertyEdit(meshProp, r, e);
					meshProp->Set(handle, r, e);
					m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMesh();
					auto component = m_component;
					UndoSystem::EndPropertyEdit(r, e, [component, &r, e](bool isUndo) { component->GetTarget<MeshRenderComponent>(r, e)->ReloadMesh(); });

					EditorSystem::Get().SetSceneDirty();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	auto materialProp = m_component->GetProperty("MaterialAsset");
	if (materialProp)
	{
		AssetHandle materialHandle = std::any_cast<AssetHandle>(materialProp->Get(r, e));
		EditResult result = EditorProperty<AssetHandle>("Material", materialHandle).Draw();
		if (result.Changed)
		{
			materialProp->Set(materialHandle, r, e);
		}
		if (result.EditStarted)
		{
			UndoSystem::BeginPropertyEdit(materialProp, r, e);
		}
		if (result.EditEnded)
		{
			UndoSystem::EndPropertyEdit(r, e);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Item"))
			{
				AssetHandle handle = *(AssetHandle*)payload->Data;
				std::filesystem::path path = AssetRegistry::Get().GetFullPath(handle);
				if (path.extension() == ".mat")
				{
					UndoSystem::BeginPropertyEdit(materialProp, r, e);
					materialProp->Set(handle, r, e);
					m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMaterial();
					UndoSystem::EndPropertyEdit(r, e, [this, &r, e](bool isUndo) { m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMaterial(); });

					EditorSystem::Get().SetSceneDirty();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}