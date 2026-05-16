#include "Inspectors/MeshRenderInspector.h"
#include <nfd.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Core/Application.h>

void MeshRenderInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	auto meshProp = m_component->GetProperty("Mesh");
	if (meshProp)
	{
		std::string meshPath = std::any_cast<std::string>(meshProp->Get(r, e));
		if (EditorProperty<std::string>("Mesh", meshPath).Draw())
		{
			meshProp->Set(meshPath, r, e);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset_Item"))
			{
				const wchar_t* pathString = (const wchar_t*)payload->Data;
				auto path = Application::GetResourcePath() / pathString;
				Assimp::Importer importer;
				if (importer.IsExtensionSupported(path.extension().string().c_str()))
				{
					meshProp->Set(path.string(), r, e);
					m_component->Initialise(r, e);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}