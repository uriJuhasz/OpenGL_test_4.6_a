#pragma once

#include <Geometry/Mesh.h>

#include <GL/glew.h>
#include <string>

GLint glsGetUInt(GLenum e);
void glsCheckShaderErrors(const std::string& shaderType, const GLuint s);
void glsCheckShaderProgramErrors(const std::string& shaderType, const GLuint p);
void glsCheckErrors();

GLuint glsGenAndBindBuffer(GLenum bufferType);

GLuint glsGenAndBindVertexArrayObject();
template<unsigned int D> GLuint glsMakeBuffer(const std::vector<Vector<D>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<2>(const std::vector<Vector<2>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<3>(const std::vector<Vector<3>>& vs, const int attributeIndex);
template GLuint glsMakeBuffer<4>(const std::vector<Vector<4>>& vs, const int attributeIndex);
