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

GLuint insertMesh(const Mesh& mesh)
{
    const int numVertices = mesh.numVertices();
    const auto& vertices = mesh.m_vertices;

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Vertex positions
    GLuint vertexBuffer = 0;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    checkGLErrors();

    //Vertex normals
    const auto& normals = mesh.m_normals;
    assert(normals.size() == numVertices);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
    checkGLErrors();

    //UV coords
    const auto& uvCoords = mesh.m_textureCoords;
    assert(uvCoords.size() == numVertices);

    GLuint uvcoordBuffer;
    glGenBuffers(1, &uvcoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvcoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(uvCoords[0]), uvCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, nullptr);
    checkGLErrors();

    //Faces
    const auto& faces = mesh.m_faces;
    const int numFaces = mesh.numFaces();
    GLuint fao;
    glGenBuffers(1, &fao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fao);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * sizeof(faces[0]), faces.data(), GL_STATIC_DRAW);

    checkGLErrors();
    return vao;
}

Vector2 glGetViewportDimensions()
{
    array<int, 4> viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());
    const float vpw = static_cast<float>(viewport[2] - viewport[0]);
    const float vph = static_cast<float>(viewport[3] - viewport[1]);
    return Vector2(vpw, vph);
}


static std::string shaderBasePath;
void setShaderBasePath(const std::string path)
{
    shaderBasePath = path;
}

string loadShader(const string& fileName)
{
    ifstream f(fileName);
    string r;
    if (f)
    {
        stringstream buffer;
        buffer << f.rdbuf();
        r = buffer.str();
    }

    return r;
}
GLuint makeSingleShader(const GLenum  shaderType, const string& shaderPath, const string& title)
{
    const auto shader = glCreateShader(shaderType);
    const auto shaderSource = loadShader(shaderBasePath + shaderPath);
    const auto shaderSourcePtr = shaderSource.c_str();
    glShaderSource(shader, 1, &shaderSourcePtr, nullptr);
    glCompileShader(shader);
    checkShaderErrors(title, shader);

    checkGLErrors();
    return shader;
}
GLuint makeShaderProgram(const string& vertexShaderFilename, const string& fragmentShaderFilename, const string& geometryShaderFilename, const string& title)
{
    const auto shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, makeSingleShader(GL_VERTEX_SHADER, vertexShaderFilename, title + "_Vertex"));
    glAttachShader(shaderProgram, makeSingleShader(GL_FRAGMENT_SHADER, fragmentShaderFilename, title + "_Fragment"));

    if (!geometryShaderFilename.empty())
    {
        glAttachShader(shaderProgram, makeSingleShader(GL_GEOMETRY_SHADER, geometryShaderFilename, title + "_Geometry"));
    }


    glLinkProgram(shaderProgram);
    checkShaderProgramErrors(title, shaderProgram);

    checkGLErrors();

    return shaderProgram;
}
GLuint makeSingleShaderCC(const GLenum  shaderType, const string& shaderSource)
{
    const auto shader = glCreateShader(shaderType);
    const auto ccString = string(
        (shaderType == GL_VERTEX_SHADER) ? "VS" :
        (shaderType == GL_TESS_CONTROL_SHADER) ? "TCS" :
        (shaderType == GL_TESS_EVALUATION_SHADER) ? "TES" :
        (shaderType == GL_GEOMETRY_SHADER) ? "GS" :
        (shaderType == GL_FRAGMENT_SHADER) ? "FS" :
        "");
    const auto defineString = "#version 430\n#define COMPILING_" + ccString + "\n";
    array<const char*, 2> ptrs = {
        defineString.c_str(),
        shaderSource.c_str()
    };
    glShaderSource(shader, 2, ptrs.data(), nullptr);
    glCompileShader(shader);
    checkShaderErrors(ccString, shader);

    checkGLErrors();
    return shader;
}
GLuint makeTessellationShaderProgram(const string& fileName, const string& title)
{
    const auto shaderSource = loadShader(shaderBasePath + fileName);
    const auto shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, makeSingleShaderCC(GL_VERTEX_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_TESS_CONTROL_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_TESS_EVALUATION_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_GEOMETRY_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_FRAGMENT_SHADER, shaderSource));
    glLinkProgram(shaderProgram);
    checkShaderProgramErrors(title, shaderProgram);

    checkGLErrors();

    return shaderProgram;
}
