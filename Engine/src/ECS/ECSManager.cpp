#include "ECS/ECSManager.h"

void ECSManager::Start(const SubsystemParams& params)
{
	const ECSManagerProps& props = static_cast<const ECSManagerProps&>(params);
	m_sceneSystem = props.SceneSystem;
	m_inputSystem = props.InputSystem;
}

void ECSManager::PreAppUpdate()
{
	if (!m_hasInitSystems)
	{
		for (auto system : m_systems)
		{
			system->Initialise(m_sceneSystem->GetActiveScene().Registry, m_inputSystem);
		}
		m_hasInitSystems = true;
	}

	for (auto system : m_systems)
	{
		system->Update(m_sceneSystem->GetActiveScene().Registry, Window::Get().DeltaTime(), m_inputSystem);
	}
}

void ECSManager::Shutdown()
{
}

void ECSManager::AddSystem(std::shared_ptr<ISystem> system)
{
	m_systems.push_back(system);
}
