#pragma once

#include <Core/Subsystem.h>

#include <GLFW/glfw3.h>
#include <memory>
#include <Rendering/FBO.h>

struct EditorSystemProps : public SubsystemParams
{
	GLFWwindow* GLFWInstance;
	// For later use when viewport windows are implemented
	//std::shared_ptr<FBO> GameRenderTarget;
};

class EditorSystem : public SingletonSubsystem<EditorSystem>
{
public:
	void Start(const SubsystemParams& params) override;
	void Shutdown() override;

	void OnAppUpdate() override;

private:
	std::shared_ptr<FBO> m_GameRenderTarget;
};