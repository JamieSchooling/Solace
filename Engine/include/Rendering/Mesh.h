#pragma once

#include "Rendering/VAO.h"
#include <memory>

struct Mesh
{
	std::shared_ptr<VAO> Geometry;
	std::shared_ptr<VAO> DepthGeometry;
};