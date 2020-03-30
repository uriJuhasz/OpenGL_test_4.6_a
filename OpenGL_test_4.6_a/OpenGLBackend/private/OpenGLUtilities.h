#pragma once

#include <Geometry/Mesh.h>

#include <GL/glew.h>

#include <array>
#include <vector>
#include <string>

GLint glsGetUInt(GLenum e);
void glsCheckShaderErrors(const std::string& shaderType, const GLuint s);
void glsCheckShaderProgramErrors(const std::string& shaderType, const GLuint p);
void glsCheckErrors();

GLuint glsGenAndBindBuffer(GLenum bufferType);

GLuint glsCreateVertexArrayObject();
template<unsigned int D> GLuint glsMakeBuffer(const std::vector<Vector<D>>& vs, const int attributeIndex);

GLuint glsCreateBuffer();
template<unsigned int D> GLuint glsCreateBuffer(const std::vector<Vector<D>>& values);

template<unsigned int D> void glsAttachBufferToAttribute(const GLuint vertexArrayObjectID, const GLuint bufferID, const int attributeIndex);

template<unsigned int D> GLuint glsCreateAndAttachBufferToAttribute(const GLuint vertexArrayObjectID, const int attributeIndex, const std::vector<Vector<D>>& values);

GLuint glsCreateAndAttachTriangleStripBuffer(const GLuint vertexArrayObjectID, const std::vector<int>& values);
GLuint glsCreateAndAttachFaceBuffer(const GLuint vertexArrayObjectID, const std::vector<Mesh::Face>& values);
GLuint glsCreateAndAttachEdgeBuffer(const GLuint vertexArrayObjectID, const std::vector<std::array<int,2>>& values);

int glsGetVertexAttribInt(const GLenum type, const int attributeIndex);

void glsDeleteVertexArrayObjectAndAllBuffers(GLuint vertexArrayObject, int maxBufferIndex);