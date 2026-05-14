#pragma once

#include "Rendering/Material.h"
#include "Rendering/VAO.h"

#include "Reflection/Reflection.h"

struct MeshRenderComponent
{
	INIT_REFLECTION(MeshRenderComponent)

	std::shared_ptr<Material> Material;
	std::shared_ptr<VAO> Geometry;
	std::string Mesh;
	PROPERTY(Mesh, PropertyType::String)

	void Initialise();

	REFLECT(MeshRenderComponent)
};