#pragma once
#include "Math/Vector.h"

#include <vector>
#include <memory>

class BezierPatch final
{
public:
	BezierPatch(const std::array<Vector3,16>& vertices)
		: m_vertices(vertices) 
	{}

	const std::array<Vector3,16>& getVertices() const { return m_vertices; }

private:
	std::array<Vector3,16> m_vertices;
};

