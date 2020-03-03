#ifndef Mesh_H_
#define Mesh_H_

#include <Math.h>
#include <array>
#include <vector>
using std::array;
using std::vector;

class Mesh final
{
public:
    Mesh() {}

public:
public:
    typedef vector<Vector3> Vertices;
    typedef vector<Vector2> UVCoords;
    typedef vector<Vector3> Normals;

    class Face final
    {
    public:
        array<int, 3> m_vis;
    };

    typedef vector<Face> Faces;

public:
    Vertices m_vertices;
    Normals  m_normals;
    UVCoords m_uvCoords;

    Faces m_faces;

public:
    class Edge final
    {
        array<int, 2> m_vis;
        array<int, 2> m_fis;
    };
};

#endif