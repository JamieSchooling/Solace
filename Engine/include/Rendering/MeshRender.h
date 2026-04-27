#pragma once

#include "Rendering/Material.h"
#include "Rendering/VAO.h"

struct MeshRenderComponent
{
	MeshRenderComponent(std::shared_ptr<Material> material, std::shared_ptr<VAO> geometry)
		: Material(material), Geometry(geometry) {}

	std::shared_ptr<Material> Material;
	std::shared_ptr<VAO> Geometry;
};