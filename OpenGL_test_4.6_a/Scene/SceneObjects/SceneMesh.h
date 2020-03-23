#pragma once
#include "SceneSurface.h"
#include "Geometry/Mesh.h"
#include "Style/ColorRGB.h"

#include <vector>

class SceneMesh :
	public virtual SceneSurface
{
public:
	virtual void setSingleEdgeVisibility(const int edgeIndex, const bool isVisible) = 0;

public:
	virtual const Mesh& getMesh() const = 0;

	virtual const std::vector<bool> getEdgeVisibility() const = 0;

public:
	virtual SceneMesh& createInstance() const = 0;
};

