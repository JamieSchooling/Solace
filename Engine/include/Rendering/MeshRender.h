#pragma once

#include "Rendering/Material.h"
#include "Rendering/VAO.h"

#include "Reflection/Reflection.h"
#include <Assets/AssetRegistry.h>

struct MeshRenderComponent
{
	INIT_REFLECTION(MeshRenderComponent)

	std::shared_ptr<Material> Material;
	std::shared_ptr<VAO> Geometry;
	AssetHandle Mesh;
	PROPERTY(Mesh)
	AssetHandle MaterialAsset;
	PROPERTY(MaterialAsset)

	void Initialise();
	void ReloadMesh();
	void ReloadMaterial();

	REFLECT(MeshRenderComponent)
};