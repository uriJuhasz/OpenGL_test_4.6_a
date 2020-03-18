#pragma once

#include "OpenGLPrimitive.h"

#include "Geometry/Mesh.h"

#include <GL/glew.h>

class OpenGLMeshPrimitive final 
	: public OpenGLPrimitive
{
public:
	OpenGLMeshPrimitive(OpenGLScene&,const Mesh& mesh);
	~OpenGLMeshPrimitive();

public:
	void render(const bool renderFaces, const bool renderEdges) const;

public:
	OpenGLStandardShaderProgram& getFaceShader() const;
	OpenGLStandardShaderProgram& getEdgeShader() const;

private:
	void insertMesh(const Mesh& mesh);
	template<unsigned int D> GLuint makeAndRegisterBuffer(const std::vector<Vector<D>>& vs, const int attributeIndex);

private:
	int m_numFaces = 0;
	int m_numEdges = 0;

	GLuint m_vertexArrayObjectIDForFaces = GL_INVALID_INDEX;
	GLuint m_vertexArrayObjectIDForEdges = GL_INVALID_INDEX;
};

