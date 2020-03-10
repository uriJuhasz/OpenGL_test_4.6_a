#pragma once
#include "Backend/BackendMesh.h"
#include "Geometry/Mesh.h"
#include "OpenGLShaderProgram.h"

#include <GL/glew.h>

class OpenGLMesh final 
	: public BackendMesh
{
public:
	OpenGLMesh(const Mesh& mesh);
	~OpenGLMesh();

	void setFaceShader(const BackendStandardShaderProgram* faceShader) override;
	void setEdgeShader(const BackendStandardShaderProgram* edgeShader) override;
public:
	void render(const bool renderFaces, const bool renderEdges) override;

private:
	void insertMesh(const Mesh& mesh);
	template<unsigned int D> GLuint makeAndRegisterBuffer(const vector<Vector<D>>& vs, const int attributeIndex);

private:
	int m_numFaces = 0;

	GLuint m_vertexArrayObjectID = GL_INVALID_INDEX;
	const OpenGLStandardShaderProgram* m_faceShader = nullptr;
	const OpenGLStandardShaderProgram* m_edgeShader = nullptr;

	vector<GLuint> m_buffers;
};

