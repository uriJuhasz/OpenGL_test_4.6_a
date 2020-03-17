#include "OpenGLSphere.h"

OpenGLSphere::OpenGLSphere(OpenGLWindow& window, const float radius)
	: m_window(window)
	, m_radius(radius)
{
}

void OpenGLSphere::render()
{
    auto& shader = m_window.getSphereEdgeShader();
	Vector4 vertex(m_modelMatrix.at(0,3), m_modelMatrix.at(1, 3), m_modelMatrix.at(2, 3), m_radius);
    if (m_edgesVisible)
    {
        glUseProgram(shader.m_shaderProgramID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(m_edgesWidth);
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(shader.m_shaderProgramID);
	    glBegin(GL_PATCHES);
	    glVertex4fv(vertex.data());
	    glEnd();
    }

	
}
