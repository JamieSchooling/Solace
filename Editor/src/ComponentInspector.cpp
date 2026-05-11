#include "ComponentInspector.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

ComponentInspector::ComponentInspector(std::shared_ptr<IComponentReflection> component)
	: m_component(component)
{
}

void ComponentInspector::Draw(entt::registry& r, entt::entity e)
{
	ImGui::Separator();
	ImGui::TextUnformatted(m_component->Name());
	DrawInspector(r, e);
	ImGui::Separator();
}

void ComponentInspector::DrawInspector(entt::registry& r, entt::entity e)
{
	for (auto& property : m_component->GetProperties())
	{
		if (property->Type() == PropertyType::Bool)
		{
			bool value = std::any_cast<bool>(property->Get(r, e));
			if (EditorProperty<bool>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Int)
		{
			int value = std::any_cast<int>(property->Get(r, e));
			if (EditorProperty<int>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Float)
		{
			float value = std::any_cast<float>(property->Get(r, e));
			if (EditorProperty<float>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec2)
		{
			glm::vec2 value = std::any_cast<glm::vec2>(property->Get(r, e));
			if (EditorProperty<glm::vec2>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec3)
		{
			glm::vec3 value = std::any_cast<glm::vec3>(property->Get(r, e));
			if (EditorProperty<glm::vec3>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Vec4)
		{
			glm::vec4 value = std::any_cast<glm::vec4>(property->Get(r, e));
			if (EditorProperty<glm::vec4>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::Quaternion)
		{
			glm::quat value = std::any_cast<glm::quat>(property->Get(r, e));
			if (EditorProperty<glm::quat>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
		else if (property->Type() == PropertyType::String)
		{
			std::string value = std::any_cast<std::string>(property->Get(r, e));
			if (EditorProperty<std::string>(property->Name(), value).Draw())
			{
				property->Set(value, r, e);
			}
		}
	}
}
