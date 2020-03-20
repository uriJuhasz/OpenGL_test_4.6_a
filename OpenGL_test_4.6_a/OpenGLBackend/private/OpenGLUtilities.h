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


GLuint glsCreateBuffer();
template<unsigned int D> GLuint glsCreateBuffer(const std::vector<Vector<D>>& values);
template GLuint glsCreateBuffer<2>(const std::vector<Vector<2>>& values);
template GLuint glsCreateBuffer<3>(const std::vector<Vector<3>>& values);
template GLuint glsCreateBuffer<4>(const std::vector<Vector<4>>& values);

template<unsigned int D> void glsAttachBufferToAttribute(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);
template void glsAttachBufferToAttribute<2>(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);
template void glsAttachBufferToAttribute<3>(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);
template void glsAttachBufferToAttribute<4>(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);

template<unsigned int D> GLuint glsCreateAndAttachBufferToAttribute(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<D>>& values);
template GLuint glsCreateAndAttachBufferToAttribute<2>(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<2>>& values);
template GLuint glsCreateAndAttachBufferToAttribute<3>(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<3>>& values);
template GLuint glsCreateAndAttachBufferToAttribute<4>(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<4>>& values);

int glGetVertexAttribInt(const GLenum type, const int attributeIndex);

void deleteVertexArrayObjectAndAllBuffers(GLuint vertexArrayObject, int maxBufferIndex);