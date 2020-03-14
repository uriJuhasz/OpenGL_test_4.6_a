#ifndef Mesh_H_
#define Mesh_H_

#include "Math/Vector.h"
#include "Math/Matrix.h"

#include <array>
#include <vector>
#include <cassert>

class Mesh final
{
public:
    Mesh() {}

    int numVertices() const { return static_cast<int>(m_vertices.size()); }
    int numFaces   () const { return static_cast<int>(m_faces.size()); }
    int numEdges   () const { return static_cast<int>(m_edges.size()); }

public:
    typedef int VertexIndex;
    typedef int FaceIndex;
    typedef int EdgeIndex;
    typedef std::vector<Vector3> Vertices;
    typedef std::vector<Vector2> UVCoords;
    typedef std::vector<Vector3> Normals;

    class Face final
    {
    public:
        Face(const int vi0, const int vi1, const int vi2) : m_vis{ vi0,vi1,vi2 } {}
        Face(const std::array<int, 3>& vis) : m_vis{ vis } {}

        std::array<int, 3> m_vis;
    };

    typedef std::vector<Face> Faces;

public:
    Vertices m_vertices;
    Normals  m_normals;
    UVCoords m_textureCoords;

    Faces m_faces;

public:
    class Edge final
    {
    public:
#pragma warning(disable:26495)
        Edge() {};
#pragma warning(default:26495)
        Edge(const int vi0,const int vi1,const int fi0,const int fi1)
            : m_vis{ vi0,vi1 }, m_fis{ fi0,fi1 }
        {}
        std::array<int, 2> m_vis;
        std::array<int, 2> m_fis;
    };
    class FaceEdges final
    {
    public:
        FaceEdges() : m_feis { -1, -1, -1 }{}
        std::array<int, 3> m_feis;
    };
    typedef std::vector<Edge> Edges;
    typedef std::vector<FaceEdges> FaceEdgess;

    FaceEdgess m_faceEdges;
    Edges m_edges;
    std::vector<EdgeIndex> m_allVertexEdgeLists;
    class VertexEdgeIndicesRange final
    {
    public:
        VertexEdgeIndicesRange()
            : m_first(0)
            , m_num(0)
        {}
        VertexEdgeIndicesRange(const int first, const int num) 
            : m_first(first)
            , m_num(num)
        {
            assert(0 <= first);
            assert(0 <= num);
        }
    public:
        int m_first;
        int m_num;
    };
    std::vector<VertexEdgeIndicesRange> m_vertexEdgeIndicesRanges;

    void calculateTopology();
    void clearTopology();

    void validateTopology();
};

#endif