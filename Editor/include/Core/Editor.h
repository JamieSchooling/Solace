#pragma once

#include <Core/Application.h>
#include <Rendering/RenderSystem.h>

class Editor : public Application
{
public:
	void Initialise(std::vector<std::string> args) override;
	void Run() override;
	void Shutdown() override;

private:
	std::shared_ptr<FBO> m_gameViewTarget;
	std::shared_ptr<FBO> m_editorViewTarget;
};