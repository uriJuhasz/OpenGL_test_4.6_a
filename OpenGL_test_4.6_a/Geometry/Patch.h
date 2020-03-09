#pragma once
#include "Math/Vector.h"

#include <vector>
#include <memory>

class VertexArrayDispatcher
{
public:
	virtual ~VertexArrayDispatcher() {}
	virtual void dispatch(const std::vector<Vector3>&) = 0;
	virtual void dispatch(const std::vector<Vector4>&) = 0;
};

class VertexArray
{
public:
	virtual int numVertices() const = 0;
	virtual void dispatch(VertexArrayDispatcher& dispatcher) const = 0;
};

template<class Vertex> class VertexArrayT final : public VertexArray
{
public:
	typedef std::vector<Vertex> Vertices;
	VertexArrayT(const Vertices& vertices)
		: m_vertices(vertices)
	{
	}

	const Vertices& getVertices() const { return m_vertices; }
	int numVertices() const override { return static_cast<int>(m_vertices.size()); }
	void dispatch(VertexArrayDispatcher& dispatcher) const override
	{
		return dispatcher.dispatch(getVertices());
	}
public:
	Vertices m_vertices;
private:
};

typedef VertexArrayT<Vector3> VertexArray3f;
typedef VertexArrayT<Vector4> VertexArray4f;

class Patch final
{
public:
	Patch(const int numVerticesPerPatch, std::unique_ptr<VertexArray> vertices) 
		: m_numVerticesPerPatch(numVerticesPerPatch)
		, m_vertices(std::move(vertices)) 
	{}

	const VertexArray& getVertices() const { return *m_vertices; }
	int getNumVerticesPerPatch() const { return m_numVerticesPerPatch; }

private:
	int m_numVerticesPerPatch;
	std::unique_ptr<VertexArray> m_vertices;
};

