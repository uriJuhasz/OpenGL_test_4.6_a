#include "OpenGLBezierPatchPrimitive.h"

#include "OpenGLBackend/private/OpenGLUtilities.h"
#include "OpenGLBackend/private/OpenGLShaderProgram.h"

#include "Utilities/Misc.h"
#include <vector>

#include <iostream>
using namespace std;

using std::vector;
using std::array;

OpenGLBezierPatchPrimitive::OpenGLBezierPatchPrimitive(OpenGLScene& scene, const std::shared_ptr<const BezierPatch> patchPtr)
    : OpenGLPrimitive(scene)
    , m_patchPtr(patchPtr)
{
    const auto& patch = *patchPtr;
    m_vertexArrayObject = glsCreateVertexArrayObject();
    glsCreateAndAttachBufferToAttribute(m_vertexArrayObject, 0, toVector(patch.getVertices()));
}

OpenGLBezierPatchPrimitive::~OpenGLBezierPatchPrimitive()
{
    glsDeleteVertexArrayObjectAndAllBuffers(m_vertexArrayObject, 1);
}

constexpr int c_numVerticesPerPatch = 16;
void OpenGLBezierPatchPrimitive::render(const bool renderFaces, const bool renderEdges) const
{
    glBindVertexArray(m_vertexArrayObject);
    glPatchParameteri(GL_PATCH_VERTICES, c_numVerticesPerPatch);

    if (renderFaces)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (m_cullBackfaces)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 0.0f);

        glUseProgram(m_scene.getBezierPatchFaceShader().m_shaderProgramID);
        glDrawArrays(GL_PATCHES, 0, c_numVerticesPerPatch);
        glUseProgram(0);
    }
    if (renderEdges)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(m_scene.getBezierPatchEdgeShader().m_shaderProgramID);
        glDrawArrays(GL_PATCHES, 0, c_numVerticesPerPatch);
        glUseProgram(0);
    }
}

OpenGLTessellationShaderProgram& OpenGLBezierPatchPrimitive::getFaceShader() const
{
    return m_scene.getBezierPatchFaceShader();
}
OpenGLTessellationShaderProgram& OpenGLBezierPatchPrimitive::getEdgeShader() const
{
    return m_scene.getBezierPatchEdgeShader();
}

const BezierPatch& OpenGLBezierPatchPrimitive::getPatch() const
{
    return *m_patchPtr;
}
