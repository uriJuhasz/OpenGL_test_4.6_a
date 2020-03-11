#ifndef Mesh_H_
#define Mesh_H_

#include "Math/Vector.h"
#include "Math/Matrix.h"

#include <array>
#include <vector>
using std::array;
using std::vector;

class Mesh final
{
public:
    Mesh() {}

    int numVertices() const { return static_cast<int>(m_vertices.size()); }
    int numFaces   () const { return static_cast<int>(m_faces.size()); }

public:
    typedef vector<Vector3> Vertices;
    typedef vector<Vector2> UVCoords;
    typedef vector<Vector3> Normals;

    class Face final
    {
    public:
        Face(const int vi0, const int vi1, const int vi2) : m_vis{ vi0,vi1,vi2 } {}
        Face(const array<int, 3>& vis) : m_vis{ vis } {}

        array<int, 3> m_vis;
    };

    typedef vector<Face> Faces;

public:
    Vertices m_vertices;
    Normals  m_normals;
    UVCoords m_textureCoords;

    Faces m_faces;

public:
    class Edge final
    {
    public:
        Edge(const int vi0,const int vi1,const int fi0,const int fi1)
            : m_vis{ vi0,vi1 }, m_fis{ fi0,fi1 }
        {}
        array<int, 2> m_vis;
        array<int, 2> m_fis;
    };

    void calculateTopology();
};

#endif