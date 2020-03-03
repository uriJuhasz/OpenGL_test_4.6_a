#pragma once

#include "Mesh.h"

#include <memory>
#include <string>

class MeshLoader
{
public:
	static std::unique_ptr<Mesh> loadMesh(const std::string& fileName);
};

