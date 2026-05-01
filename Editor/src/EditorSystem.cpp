#include "EditorSystem.h"

#include <Scenes/Scene.h>
#include <Scenes/SceneSystem.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <Scenes/NameComponent.h>

#include <filesystem>
#include <Core/Application.h>
#include <Transform/Transform.h>

void EditorSystem::Start(const SubsystemParams& params)
{
	const EditorSystemProps& props = static_cast<const EditorSystemProps&>(params);

	GLFWwindow* window = props.GLFWInstance;
	
	// For later use when viewport windows are implemented
	//m_GameRenderTarget = props.GameRenderTarget;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	if (!std::filesystem::exists(Application::GetConfigPath() / "Layouts"))
	{
		std::filesystem::create_directory(Application::GetConfigPath() / "Layouts");
	}

	if (std::filesystem::exists(Application::GetConfigPath() / "Layouts" / "Active.ini"))
	{
		ImGui::LoadIniSettingsFromDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
		m_NewLayout = LayoutOption::Active;
	}
	else
	{
		ImGui::LoadIniSettingsFromDisk((Application::GetResourcePath() / "Layouts" / "Default.ini").string().c_str());
		m_NewLayout = LayoutOption::Default;
	}

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void EditorSystem::Shutdown()
{
	ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
}

void EditorSystem::PreAppUpdate()
{
	HandleLayoutChange(); // Done on frame start to preserve docking in layout
}

void EditorSystem::OnAppUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);

	DrawMenuBar();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Debug Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	// Framework is there to make game render to an ImGui window (Editor Viewport Window), 
	// for now I use a dockspace with a passthrough central node, with the scene rendered to the default framebuffer
	//ImGui::Begin("Game");
	//GLuint textureID = m_GameRenderTarget->GetTarget()->GetID();
	//ImVec2 uv0 = ImVec2(0.0f, 1.0f);  // Top-left (flipped)
	//ImVec2 uv1 = ImVec2(1.0f, 0.0f);  // Bottom-right (flipped)
	//ImVec2 imageSize = ImGui::GetContentRegionAvail();  // (width, height)
	//ImGui::Image((void*)(intptr_t)textureID, imageSize, uv0, uv1);
	//ImGui::End();

	Scene& scene = SceneSystem::Get().GetActiveScene();
	DrawSceneHierarchy(scene);
	DrawInspector(scene.Registry);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorSystem::HandleLayoutChange()
{
	if (m_NewLayout == m_CurrentLayout) return;

	if (m_CurrentLayout == LayoutOption::Active)
	{
		ImGui::SaveIniSettingsToDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
	}

	switch (m_NewLayout)
	{
	case LayoutOption::Default:
		ImGui::LoadIniSettingsFromDisk((Application::GetResourcePath() / "Layouts" / "Default.ini").string().c_str());
		break;
	case LayoutOption::Active:
		ImGui::LoadIniSettingsFromDisk((Application::GetConfigPath() / "Layouts" / "Active.ini").string().c_str());
		break;
	}

	m_CurrentLayout = m_NewLayout;
}

void EditorSystem::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Menu Item"))
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Menu Item"))
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Layout"))
		{
			if (ImGui::MenuItem("Default", nullptr, m_CurrentLayout == LayoutOption::Default))
			{
				m_NewLayout = LayoutOption::Default;
			}

			if (ImGui::MenuItem("Active", nullptr, m_CurrentLayout == LayoutOption::Active))
			{
				m_NewLayout = LayoutOption::Active;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorSystem::DrawSceneHierarchy(Scene& scene)
{
	ImGui::Begin("Hierarchy");

	ImGuiTreeNodeFlags baseNodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx(scene.Name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
	{
		for (auto& entity : scene.Registry.view<entt::entity>())
		{
			std::string name = "Unnamed Entity";
			if (scene.Registry.all_of<NameComponent>(entity))
			{
				name = scene.Registry.get<NameComponent>(entity).Name;
			}
			ImGui::PushID((int)entity);
			ImGuiTreeNodeFlags nodeFlags = baseNodeFlags;
			if (entity == m_SelectedEntity)
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx(name.c_str(), nodeFlags))
			{
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					m_SelectedEntity = entity;
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	// Check for deselection
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
	{
		m_SelectedEntity = entt::null;
	}

	ImGui::End();
}

glm::vec3 MakeContinuous(glm::vec3 current, glm::vec3 previous)
{
	for (int i = 0; i < 3; ++i)
	{
		float delta = current[i] - previous[i];

		if (delta > 180.0f) current[i] -= 360.0f;
		if (delta < -180.0f) current[i] += 360.0f;
	}
	return current;
}


void EditorSystem::DrawInspector(entt::registry& registry)
{
	ImGui::Begin("Inspector");
	if (m_SelectedEntity != entt::null)
	{
		ImGui::PushID((int)m_SelectedEntity);
		if (registry.all_of<NameComponent>(m_SelectedEntity))
		{
			if (ImGui::BeginTable("##NameProperty", 2))
			{
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Name");

				ImGui::TableSetColumnIndex(1);
				ImGui::InputText("##Name", &registry.get<NameComponent>(m_SelectedEntity).Name);

				ImGui::EndTable();
			}
		}
		if (registry.all_of<Transform>(m_SelectedEntity))
		{
			Transform& transform = registry.get<Transform>(m_SelectedEntity);

			ImGui::Separator();
			ImGui::TextUnformatted("Transform");
			if (ImGui::BeginTable("##PosProperty", 2))
			{
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Position");

				ImGui::TableSetColumnIndex(1);
				ImGui::DragFloat3("##Position", &transform.Position.x, 0.1f);

				ImGui::EndTable();
			}
			if (ImGui::BeginTable("##RotProperty", 2))
			{
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Rotation");

				ImGui::TableSetColumnIndex(1); 
				glm::quat& rotation = transform.Rotation;

				if (!m_EulerCache.contains(m_SelectedEntity))
				{
					m_EulerCache[m_SelectedEntity] = glm::degrees(transform.EulerAngles());
				}

				glm::vec3 euler = m_EulerCache[m_SelectedEntity];

				if (ImGui::DragFloat3("Rotation", &euler.x))
				{
					glm::vec3 radianAngles = glm::radians(euler);

					glm::quat x = glm::angleAxis(radianAngles.x, glm::vec3(1, 0, 0));
					glm::quat y = glm::angleAxis(radianAngles.y, glm::vec3(0, 1, 0));
					glm::quat z = glm::angleAxis(radianAngles.z, glm::vec3(0, 0, 1));
					rotation = glm::normalize(x * y * z);
					m_EulerCache[m_SelectedEntity] = euler;
				}

				ImGui::EndTable();
			}
			if (ImGui::BeginTable("##ScaleProperty", 2))
			{
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("Scale");

				ImGui::TableSetColumnIndex(1);
				ImGui::DragFloat3("##Scale", &transform.Scale.x, 0.1f);

				ImGui::EndTable();
			}
			ImGui::Separator();
		}
		ImGui::PopID();
	}
	ImGui::End();
}
