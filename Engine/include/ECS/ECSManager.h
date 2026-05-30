#pragma once

#include "Core/Subsystem.h"
#include "ECS/System.h"
#include <Scenes/SceneSystem.h>

struct ECSManagerProps : public SubsystemParams
{
	SceneSystem* SceneSystem = nullptr;
};

class ECSManager : public SingletonSubsystem<ECSManager>
{
public:
	void Start(const SubsystemParams& params) override;
	void PreAppUpdate() override;
	void Shutdown() override;

	void AddSystem(std::shared_ptr<ISystem> system);

private:
	std::vector<std::shared_ptr<ISystem>> m_systems;
	SceneSystem* m_sceneSystem;

	bool m_hasInitSystems = false;
};