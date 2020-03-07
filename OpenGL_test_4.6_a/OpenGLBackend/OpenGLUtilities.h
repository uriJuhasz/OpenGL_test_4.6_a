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


