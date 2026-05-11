#pragma once

#include <Core/Application.h>
#include <Rendering/RenderSystem.h>

class Editor : public Application
{
public:
	void Initialise() override;
	void Run() override;
	void Shutdown() override;
};