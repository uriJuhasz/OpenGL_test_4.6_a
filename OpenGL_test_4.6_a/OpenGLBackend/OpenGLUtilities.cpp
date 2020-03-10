#include "OpenGLUtilities.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;


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
            cerr << " OpenGL error: " << err << gluErrorString(err) << endl;
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


GLuint glsGenAndBindVertexArray()
{
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    return vao;
}

template GLuint glsMakeBuffer<2>(const vector<Vector<2>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<3>(const vector<Vector<3>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<4>(const vector<Vector<4>>& vs, const int attributeIndex);
