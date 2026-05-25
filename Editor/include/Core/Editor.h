#pragma once

#include <Core/Application.h>
#include <Rendering/RenderSystem.h>

#include "Core/ProjectManager.h"

class Editor : public Application
{
public:
	void Initialise(std::vector<std::string> args) override;
	void Run() override;
	void Shutdown() override;

private:
	std::shared_ptr<FBO> m_gameViewTarget;
	std::shared_ptr<FBO> m_editorViewTarget;

	ProjectManager m_projectManager;

	void RunProjectManager(std::vector<std::string> args);
};