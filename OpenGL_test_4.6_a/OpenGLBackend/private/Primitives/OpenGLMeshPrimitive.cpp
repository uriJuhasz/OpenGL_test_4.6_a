#include "OpenGLMeshPrimitive.h"
#include "../OpenGLWindow.h"
#include "OpenGLBackend/private/OpenGLUtilities.h"

#include "Utilities/Exception.h"
#include "Utilities/Misc.h"

#include <cassert>
#include <iostream>

using std::vector;
using std::array;

GLuint insertMesh(const Mesh& mesh);

OpenGLMeshPrimitive::OpenGLMeshPrimitive(OpenGLScene& scene, const std::shared_ptr<const Mesh> meshPtr)
    : OpenGLPrimitive(scene)
    , m_meshPtr(meshPtr)
    , m_numFaces(meshPtr->numFaces())
    , m_numEdges(meshPtr->numEdges())
{
    insertMesh(*meshPtr);
}

OpenGLMeshPrimitive::~OpenGLMeshPrimitive()
{
    glsDeleteVertexArrayObjectAndAllBuffers(m_vertexArrayObjectIDForFaces, 3);
}

const Mesh& OpenGLMeshPrimitive::getMesh() const
{
    return *m_meshPtr;
}

void OpenGLMeshPrimitive::render(const bool renderFaces, const bool renderEdges) const
{
    if (m_vertexArrayObjectIDForFaces == 0)
        throw new Exception("BackendMesh: invalid mesh cache");

    if (renderFaces)
    {
        const auto& faceShader = m_scene.getMeshFaceShader();

        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_CULL_FACE);
//        glEnable(GL_POLYGON_OFFSET_FILL);
//        glPolygonOffset(1.0f, 0.0f);
        glBindVertexArray(m_vertexArrayObjectIDForFaces);
        glUseProgram(faceShader.m_shaderProgramID);
        glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, 0);
        glUseProgram(0);
    }
    if (renderEdges)
    {
        const auto& edgeShader = m_scene.getMeshEdgeShader();

        glUseProgram(edgeShader.m_shaderProgramID);

        glDepthFunc(GL_LEQUAL);

        if (renderFaces)
        {
            glPolygonMode(GL_FRONT, GL_LINE);
            glEnable(GL_CULL_FACE);
            glBindVertexArray(m_vertexArrayObjectIDForFaces);
            glDrawElements(GL_TRIANGLES, m_numFaces*3, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glBindVertexArray(m_vertexArrayObjectIDForEdges);
            glDrawElements(GL_LINES, m_numEdges * 2, GL_UNSIGNED_INT, 0);
        }
        glUseProgram(0);
    }
}
void OpenGLMeshPrimitive::insertMesh(const Mesh& mesh)
{
    assert(mesh.m_normals.size() == mesh.numVertices());
    assert(mesh.m_textureCoords.size() == mesh.numVertices());

    const auto vertexArrayObjectIDForFaces = glsCreateVertexArrayObject();

    const auto vertexBufferID  = glsCreateAndAttachBufferToAttribute(vertexArrayObjectIDForFaces, 0, mesh.m_vertices);
    glsCreateAndAttachBufferToAttribute(vertexArrayObjectIDForFaces, 1, mesh.m_normals);
    glsCreateAndAttachBufferToAttribute(vertexArrayObjectIDForFaces, 2, mesh.m_textureCoords);

    {//Faces
        glsCreateAndAttachFaceBuffer(vertexArrayObjectIDForFaces, mesh.m_faces);
        m_vertexArrayObjectIDForFaces = vertexArrayObjectIDForFaces;
    }
    {//Edges
        const auto& edges = mesh.m_edges;
        const auto numEdges = mesh.numEdges();
        const auto vertexArrayObjectIDForEdges = glsCreateVertexArrayObject();
        glsAttachBufferToAttribute<3>(vertexArrayObjectIDForEdges, vertexBufferID, 0);
        vector<array<int,2>> edgeVertexIndices; edgeVertexIndices.reserve(numEdges);
        for (const auto& e : edges)
            edgeVertexIndices.emplace_back(e.m_vis);

        glsCreateAndAttachEdgeBuffer(vertexArrayObjectIDForEdges, edgeVertexIndices);

        m_vertexArrayObjectIDForEdges = vertexArrayObjectIDForEdges;
    }

}

OpenGLStandardShaderProgram& OpenGLMeshPrimitive::getFaceShader() const
{
    return m_scene.getMeshFaceShader();
}
OpenGLStandardShaderProgram& OpenGLMeshPrimitive::getEdgeShader() const
{
    return m_scene.getMeshEdgeShader();
}
