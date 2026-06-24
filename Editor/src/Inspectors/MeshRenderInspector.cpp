#include "Inspectors/MeshRenderInspector.h"
#include "Inspectors/EditorProperty.h"
#include <nfd.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Core/Application.h>

void MeshRenderInspector::Initialise(entt::registry& r, entt::entity e)
{
	m_meshProp = m_component->GetProperty("MeshAsset");
	m_materialProp = m_component->GetProperty("MaterialAsset");
}

void MeshRenderInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	AssetHandle meshHandle = std::any_cast<AssetHandle>(m_meshProp->Get(r, e));
	EditResult result = EditorProperty<AssetHandle>("Mesh", meshHandle).Draw();
	if (result.Changed)
	{
		m_meshProp->Set(meshHandle, r, e);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_meshProp, r, e);
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
				UndoSystem::BeginPropertyEdit(m_meshProp, r, e);
				m_meshProp->Set(handle, r, e);
				m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMesh();
				auto component = m_component;
				UndoSystem::EndPropertyEdit(r, e, [component, &r, e](bool isUndo) { component->GetTarget<MeshRenderComponent>(r, e)->ReloadMesh(); });

				EditorSystem::Get().SetSceneDirty();
			}
		}
		ImGui::EndDragDropTarget();
	}
	
	AssetHandle materialHandle = std::any_cast<AssetHandle>(m_materialProp->Get(r, e));
	result = EditorProperty<AssetHandle>("Material", materialHandle).Draw();
	if (result.Changed)
	{
		m_materialProp->Set(materialHandle, r, e);
	}
	if (result.EditStarted)
	{
		UndoSystem::BeginPropertyEdit(m_materialProp, r, e);
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
				UndoSystem::BeginPropertyEdit(m_materialProp, r, e);
				m_materialProp->Set(handle, r, e);
				m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMaterial();
				UndoSystem::EndPropertyEdit(r, e, [this, &r, e](bool isUndo) { m_component->GetTarget<MeshRenderComponent>(r, e)->ReloadMaterial(); });

				EditorSystem::Get().SetSceneDirty();
			}
		}
		ImGui::EndDragDropTarget();
	}
}