#include "OpenGLUtilities.h"

#include "Utilities/Misc.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::array;

string toString(const vector<char>& v)
{
    string r; r.reserve(v.size());
    for (const auto c : v)
        r.push_back(c);
    return r;
}

GLint glsGetUInt(GLenum e)
{
    GLint r;
    glGetIntegerv(e, &r);
    return r;
}
void glsCheckShaderErrors(const string& shaderType, const GLuint s)
{
    int infoLength;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0)
    {
        vector<char> info(infoLength, ' ');
        glGetShaderInfoLog(s, infoLength, &infoLength, info.data());
        const auto infoString = toString(info);
        cerr << shaderType << " shader log: " << endl << infoString;
    }
}
void glsCheckShaderProgramErrors(const string& shaderType, const GLuint p)
{
    int infoLength;
    glGetProgramiv(p, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0)
    {
        vector<char> info(infoLength, ' ');
        glGetProgramInfoLog(p, infoLength, &infoLength, info.data());
        const auto infoString = toString(info);
        cerr << shaderType << " shader program log: " << endl << infoString << endl;
    }
}

void glsCheckErrors()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        do {
            cerr << " OpenGL error: " << err << " - " << gluErrorString(err) << endl;
            err = glGetError();
        } while (err != GL_NO_ERROR);
    }
}

GLuint glsGenAndBindBuffer(GLenum bufferType)
{
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(bufferType, buffer);
    return buffer;
}

template<unsigned int D> GLuint glsMakeBuffer(const vector<Vector<D>>& vs, const int attributeIndex)
{
    const auto buffer = glsGenAndBindBuffer(GL_ARRAY_BUFFER);
    glNamedBufferData(buffer, vs.size() * sizeof(vs[0]), vs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(attributeIndex);
    glVertexAttribPointer(attributeIndex, D, GL_FLOAT, GL_FALSE, 0, nullptr);
    glsCheckErrors();

    return buffer;
}


GLuint glsCreateVertexArrayObject()
{
    GLuint vertexArrayObjectID = 0;
    glCreateVertexArrays(1, &vertexArrayObjectID); 
//    glGenVertexArrays(1, &vertexArrayObjectID);
//    glBindVertexArray(vertexArrayObjectID);
    return vertexArrayObjectID;
}

template GLuint glsMakeBuffer<2>(const vector<Vector<2>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<3>(const vector<Vector<3>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<4>(const vector<Vector<4>>& vs, const int attributeIndex);


GLuint glsCreateBuffer()
{
    GLuint bufferID = 0;
    glCreateBuffers(1, &bufferID);
    return bufferID;
}
template<unsigned int D>GLuint glsCreateBuffer(const vector<Vector<D>>& values)
{
    const auto valueSize = sizeof(values[0]); // sizeof(values[0]);
    const auto numValues = toInt(values.size());

    const auto bufferID = glsCreateBuffer();
    //    std::cout << "  Created buffer " << bufferID << " for attribute " << attributeIndex << std::endl;
    glNamedBufferStorage(bufferID, numValues * valueSize, values.data(), 0);
    return bufferID;
}

template<unsigned int D>void glsAttachBufferToAttribute(
    const GLuint vertexArrayObjectID,
    const GLuint bufferID,
    const int attributeIndex
    )
{
    glVertexArrayVertexBuffer(vertexArrayObjectID, attributeIndex, bufferID, 0, D * sizeof(float));
    glEnableVertexArrayAttrib(vertexArrayObjectID, attributeIndex);
    glVertexArrayAttribFormat(vertexArrayObjectID, attributeIndex, D, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vertexArrayObjectID, attributeIndex, attributeIndex);
}

template<unsigned int D>GLuint glsCreateAndAttachBufferToAttribute(
    const GLuint vertexArrayObjectID,
    const int attributeIndex,
    const vector<Vector<D>>& values
    )
{
    const auto bufferID = glsCreateBuffer(values);
    glsAttachBufferToAttribute<D>(vertexArrayObjectID, bufferID, attributeIndex);

    return bufferID;
}

int glsGetVertexAttribInt(const GLenum type, const int attributeIndex)
{
    GLint val;
    glGetVertexAttribiv(attributeIndex, type, &val);
    return val;
}

void glsDeleteVertexArrayObjectAndAllBuffers(GLuint vertexArrayObject, int maxBufferIndex)
{
    if (vertexArrayObject)
    {
        glBindVertexArray(vertexArrayObject);
        for (int i = 0; i < maxBufferIndex; ++i)
        {
            if (glsGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_ENABLED, i))
            {
                GLuint bufferID = glsGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, i);
                glDeleteBuffers(1, &bufferID);
            }

        }
        GLuint vertexIndexBufferID = 0;
        glGetVertexArrayiv(vertexArrayObject, GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&vertexIndexBufferID);
        glDeleteBuffers(1, &vertexIndexBufferID);
    }
    glDeleteVertexArrays(1, &vertexArrayObject);
}

GLuint glsCreateAndAttachIndexBuffer(const GLuint vertexArrayObjectID, const int size, const void* data)
{
    const auto indexBufferID = glsCreateBuffer();
    glNamedBufferStorage(indexBufferID, size, data, 0);
    glVertexArrayElementBuffer(vertexArrayObjectID, indexBufferID);
    return indexBufferID;
}
GLuint glsCreateAndAttachFaceBuffer(const GLuint vertexArrayObjectID, const std::vector<Mesh::Face>& faces)
{
    return glsCreateAndAttachIndexBuffer(vertexArrayObjectID, numElements(faces) * sizeof(faces[0]), faces.data());
}
GLuint glsCreateAndAttachEdgeBuffer(const GLuint vertexArrayObjectID, const vector<array<int, 2>>& edges)
{
    return glsCreateAndAttachIndexBuffer(vertexArrayObjectID, numElements(edges) * sizeof(edges[0]), edges.data());
}
GLuint glsCreateAndAttachTriangleStripBuffer(const GLuint vertexArrayObjectID, const std::vector<int>& triangleStrip)
{
    return glsCreateAndAttachIndexBuffer(vertexArrayObjectID, numElements(triangleStrip) * sizeof(triangleStrip[0]), triangleStrip.data());
}

template GLuint glsMakeBuffer<2>(const std::vector<Vector<2>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<3>(const std::vector<Vector<3>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<4>(const std::vector<Vector<4>>& vs, const int attributeIndex);


template GLuint glsCreateBuffer<2>(const std::vector<Vector<2>>& values);
template GLuint glsCreateBuffer<3>(const std::vector<Vector<3>>& values);
template GLuint glsCreateBuffer<4>(const std::vector<Vector<4>>& values);

template void glsAttachBufferToAttribute<2>(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);
template void glsAttachBufferToAttribute<3>(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);
template void glsAttachBufferToAttribute<4>(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);

template GLuint glsCreateAndAttachBufferToAttribute<2>(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<2>>& values);
template GLuint glsCreateAndAttachBufferToAttribute<3>(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<3>>& values);
template GLuint glsCreateAndAttachBufferToAttribute<4>(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<4>>& values);
