#pragma once

#include "BackendSurface.h"

class BackendMesh
	: public virtual BackendSurface
{
public:
	virtual void setEdgeVisibility(const int edgeIndex, const bool isVisible) = 0;

public:
	virtual BackendMesh& createInstance() const override = 0;
};

