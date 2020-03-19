#include "OpenGLSphere.h"

OpenGLSphere::OpenGLSphere(OpenGLScene& scene, const float radius)
	: OpenGLSurface(scene)
    , OpenGLGraphicObject(scene)
	, m_radius(radius)
{
}

SceneObject& OpenGLSphere::createInstance() const
{
    return getScene().addSphere(getCenter(), m_radius);
}

void OpenGLSphere::render()
{
    auto& shader = getScene().getSphereEdgeShader();
	Vector4 vertex(m_modelMatrix.at(0,3), m_modelMatrix.at(1, 3), m_modelMatrix.at(2, 3), m_radius);
    if (m_edgesVisible)
    {
        glUseProgram(shader.m_shaderProgramID);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        glLineWidth(m_edgesWidth);
//        glEnable(GL_CULL_FACE);
//        glDepthFunc(GL_LEQUAL);

        glBegin(GL_PATCHES);
	    glVertex4fv(vertex.data());
	    glEnd();
    }

	
}

void OpenGLSphere::setRadius(const float newRadius)
{
    m_radius = newRadius;
}

float OpenGLSphere::getRadius() const
{
    return m_radius;
}

Vector3 OpenGLSphere::getCenter() const
{
    return Vector3(m_modelMatrix.at(0, 3), m_modelMatrix.at(1, 3), m_modelMatrix.at(2, 3));
}
