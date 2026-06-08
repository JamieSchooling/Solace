#pragma once

#include <Core/Application.h>
#include <Rendering/RenderSystem.h>
#include <Assets/AssetHandle.h>

#include "Core/ProjectManager.h"

enum class EditorState
{
	Edit,
	Play
};

class Editor : public Application
{
public:
	void Initialise(std::vector<std::string> args) override;
	void Run() override;
	void Shutdown() override;

	inline static std::filesystem::path ProjectDirectoryPath() { return s_projectDirectoryPath; }
	inline static std::filesystem::path ProjectAssetsPath() { return s_projectAssetsPath; }
	inline static void SetStartupScene(AssetHandle scene) { s_startupScene = scene; }

	inline static EditorState CurrentState() { return s_state; }
	inline static void SetCurrentState(EditorState state) { s_state = state; }
protected:
	bool ShouldUpdate(const RegisteredSubsystem& subsystem) const override;
private:
	std::shared_ptr<FBO> m_gameViewTarget;
	std::shared_ptr<FBO> m_editorViewTarget;
	inline static std::filesystem::path s_projectDirectoryPath;
	inline static std::filesystem::path s_projectAssetsPath;
	inline static AssetHandle s_startupScene;

	inline static EditorState s_state = EditorState::Edit;

	ProjectManager m_projectManager;

	void SetImGuiTheme();

	void RunProjectManager(std::vector<std::string> args);
};