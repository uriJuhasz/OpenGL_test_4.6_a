#pragma once
#include "SceneObject.h"
#include "Geometry/Mesh.h"
#include "Style/ColorRGB.h"

#include <vector>

class SceneMeshObject :
	public virtual SceneObject
{
public:
	virtual void setEdgeWidth(const float) = 0;
	virtual void setEdgeColor(const ColorRGB&) = 0;
	virtual void setEdgeVisibility(const int edgeIndex, const bool isVisible) = 0;

public:
	virtual const Mesh& getMesh() const = 0;

	virtual float    getEdgeWidth() const = 0;
	virtual ColorRGB getEdgeColor() const = 0;

	virtual const std::vector<bool> getEdgeVisibility() const = 0;
};

