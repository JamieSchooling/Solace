#pragma once

#include "Windows/EditorWindow.h"
#include "Menu/MenuRegistry.h"

#include <Rendering/Material.h>
#include <filesystem>

class MaterialWindow : public EditorWindow
{
public:
	MaterialWindow() : EditorWindow("Material") {}

	static void Open();
	MENU_ITEM("Window/General/Material", 400, Open);
	
	static void Open(AssetHandle materialHandle);
protected:
	void DrawContent(entt::entity& selected, Scene& scene) override;
	void OnEvent(Event& e) override;

private:
	std::weak_ptr<Material> m_material;
	std::filesystem::path m_materialPath;

	void DrawProperty(const std::string& name, UniformData data, UniformDescription desc);
};