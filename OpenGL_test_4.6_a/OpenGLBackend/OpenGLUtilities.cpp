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

GLint glGetUInt(GLenum e)
{
    GLint r;
    glGetIntegerv(e, &r);
    return r;
}
void checkShaderErrors(const string& shaderType, const GLuint s)
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
void checkShaderProgramErrors(const string& shaderType, const GLuint p)
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

void checkGLErrors()
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


