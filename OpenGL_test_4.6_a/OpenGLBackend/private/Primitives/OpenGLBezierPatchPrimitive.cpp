#include "OpenGLBezierPatchPrimitive.h"

#include "OpenGLBackend/private/OpenGLUtilities.h"
#include "OpenGLBackend/private/OpenGLShaderProgram.h"

#include <vector>

using std::vector;

OpenGLBezierPatchPrimitive::OpenGLBezierPatchPrimitive(OpenGLScene& scene, const BezierPatch& patch)
    : OpenGLPrimitive(scene)
{
    const auto& vertices = patch.getVertices();

    m_vertexArrayObject = glsGenAndBindVertexArrayObject();

    vector<Vector3> verticesV(vertices.begin(), vertices.end());
    m_vertexBuffer = glsMakeBuffer(verticesV, 0);

    const auto numVerticesPerPatch = patch.getVertices().size();
    glPatchParameteri(GL_PATCH_VERTICES, numVerticesPerPatch);
}

OpenGLBezierPatchPrimitive::~OpenGLBezierPatchPrimitive()
{
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void OpenGLBezierPatchPrimitive::render(const bool renderFaces, const bool renderEdges) const
{
    constexpr bool showPatchFaces = true;
    if (renderFaces)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
/*        if (m_cullBackfaces)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);*/
        glDepthFunc(GL_LESS);

        glUseProgram(m_scene.getBezierPatchFaceShader().m_shaderProgramID);
        glDrawArrays(GL_PATCHES, 0, m_numVertices);
    }
    if (renderEdges)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        glLineWidth(1.0f);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0, 0.0);
        glUseProgram(m_scene.getBezierPatchEdgeShader().m_shaderProgramID);
        glDrawArrays(GL_PATCHES, 0, m_numVertices);
    }
}
