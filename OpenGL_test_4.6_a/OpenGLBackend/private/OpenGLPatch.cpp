#include "OpenGLPatch.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLBackend/OpenGLUtilities.h"

namespace {
    class OpenGLPatchDispatcher final : public VertexArrayDispatcher
    {
    public:
        virtual void dispatch(const std::vector<Vector3>& vs)
        {
            buffer = glsMakeBuffer(vs, 0);
        }
        virtual void dispatch(const std::vector<Vector4>& vs)
        {
            buffer = glsMakeBuffer(vs, 0);
        }
        GLuint buffer = 0;
    };
};
OpenGLPatch::OpenGLPatch(const Patch& patch)
{
    const auto& vertexVector = patch.getVertices();
    
    m_vertexArrayObject = glsGenAndBindVertexArrayObject();

    m_numVertices = vertexVector.numVertices();
    m_numVerticesPerPatch = patch.getNumVerticesPerPatch();
    OpenGLPatchDispatcher dispatcher;
    vertexVector.dispatch(dispatcher);
    m_vertexBuffer = dispatcher.buffer; 

    glPatchParameteri(GL_PATCH_VERTICES, m_numVerticesPerPatch);
}

OpenGLPatch::~OpenGLPatch()
{
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void OpenGLPatch::setFaceShader(const BackendTesselationShaderProgram* faceShader)
{
	m_faceShader = dynamic_cast<const OpenGLTessellationShaderProgram*>(faceShader); //Eliminating this requires path-dependent-types
}
void OpenGLPatch::setEdgeShader(const BackendTesselationShaderProgram* edgeShader)
{
	m_edgeShader = dynamic_cast<const OpenGLTessellationShaderProgram*>(edgeShader);
}

void OpenGLPatch::render(const bool renderFaces, const bool renderEdges)
{
    glBindVertexArray(m_vertexArrayObject);
    glPatchParameteri(GL_PATCH_VERTICES, m_numVerticesPerPatch);
    constexpr bool showPatchFaces = true;
    if (renderFaces && m_faceShader)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (m_cullBackfaces)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);

        glUseProgram(m_faceShader->m_shaderProgramID);
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
        glUseProgram(m_edgeShader->m_shaderProgramID);
        glDrawArrays(GL_PATCHES, 0, m_numVertices);
    }
}

void OpenGLPatch::setCullBackfaces(const bool cull)
{
    m_cullBackfaces = cull;
}
