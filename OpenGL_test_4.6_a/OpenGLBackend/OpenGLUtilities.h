#pragma once

#include <Geometry/Mesh.h>

#include <GL/glew.h>
#include <string>

using std::string;

GLint glGetUInt(GLenum e);
void checkShaderErrors(const string& shaderType, const GLuint s);
void checkShaderProgramErrors(const string& shaderType, const GLuint p);
void checkGLErrors();

GLuint insertMesh(const Mesh& mesh);

Vector2 glGetViewportDimensions();

void setShaderBasePath(const string& path);
string loadShader(const string& fileName);
GLuint makeSingleShader(const GLenum  shaderType, const string& shaderPath, const string& title);
GLuint makeShaderProgram(const string& vertexShaderFilename, const string& fragmentShaderFilename, const string& geometryShaderFilename, const string& title);
GLuint makeSingleShaderCC(const GLenum  shaderType, const string& shaderSource);
GLuint makeTessellationShaderProgram(const string& fileName, const string& title);




