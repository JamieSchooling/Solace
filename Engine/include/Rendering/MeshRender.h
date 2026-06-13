#pragma once

#include "Rendering/Material.h"
#include "Rendering/VAO.h"

#include "Reflection/Reflection.h"
#include "Rendering/Mesh.h"
#include <Assets/AssetRegistry.h>

struct MeshRenderComponent
{
	INIT_REFLECTION(MeshRenderComponent)

	std::weak_ptr<Material> Material;
	std::weak_ptr<Mesh> Mesh;
	AssetHandle MeshAsset;
	PROPERTY(MeshAsset)
	AssetHandle MaterialAsset;
	PROPERTY(MaterialAsset)

	void Initialise();
	void ReloadMesh();
	void ReloadMaterial();

	REFLECT(MeshRenderComponent)
};