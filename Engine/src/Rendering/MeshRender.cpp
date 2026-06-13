#include "Rendering/MeshRender.h"

#include "Assets/AssetCache.h"

void MeshRenderComponent::Initialise()
{
	ReloadMesh();
	ReloadMaterial();
}

void MeshRenderComponent::ReloadMesh()
{
	Mesh = AssetCache::Load<::Mesh>(MeshAsset);
}

void MeshRenderComponent::ReloadMaterial()
{
	Material = AssetCache::Load<::Material>(MaterialAsset);
}