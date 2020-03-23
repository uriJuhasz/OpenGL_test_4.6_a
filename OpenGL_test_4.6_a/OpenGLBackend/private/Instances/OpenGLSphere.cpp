#include "OpenGLSphere.h"

#include "OpenGLBackend/private/OpenGLUtilities.h"
#include <iostream>

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

constexpr int c_numVerticesPerPatch = 1;
void OpenGLSphere::renderMain()
{
    if (!isVisible())
        return;
    glBindVertexArray(0);
    glPatchParameteri(GL_PATCH_VERTICES, c_numVerticesPerPatch);

    Vector4 vertex(m_modelMatrix.at(0,3), m_modelMatrix.at(1, 3), m_modelMatrix.at(2, 3), m_radius);
    
    if (m_facesVisible)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        if (m_edgesVisible)
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 0.0f);
        }
        auto& shader = getScene().getSphereFaceShader();
/*        if (m_edgesVisible)
        {
            shader.setParameter("drawEdges", 1);
            shader.setParameter("edgeColor", m_edgesColor.m_value);
        }
        else
        {
            shader.setParameter("drawEdges", 0);
        }
        */

        glUseProgram(shader.m_shaderProgramID);

        glBegin(GL_PATCHES);
        glVertex4fv(vertex.data());
        glEnd();

        glUseProgram(0);
    }

    if (m_edgesVisible)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(m_edgesWidth);
        if (m_facesVisible)
        {
            glDepthFunc(GL_LEQUAL);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        auto& shader = getScene().getSphereEdgeShader();

        glUseProgram(shader.m_shaderProgramID);
        glBegin(GL_PATCHES);
	    glVertex4fv(vertex.data());
	    glEnd();
        glUseProgram(0);
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

OpenGLGraphicObject::BoundingBox OpenGLSphere::getBoundingBox() const
{
    const auto center = getCenter();
    const auto radius = getRadius();
    return BoundingBox(center-Vector3(radius,radius,radius),center + Vector3(radius,radius,radius));
}
