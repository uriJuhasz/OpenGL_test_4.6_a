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

OpenGLMeshPrimitive::OpenGLMeshPrimitive(OpenGLScene& scene, const Mesh& mesh)
    : OpenGLPrimitive(scene)
    , m_numFaces(mesh.numFaces())
    , m_numEdges(mesh.numEdges())
{
    insertMesh(mesh);
}

OpenGLMeshPrimitive::~OpenGLMeshPrimitive()
{
    deleteVertexArrayObjectAndAllBuffers(m_vertexArrayObjectIDForFaces, 3);
}

void OpenGLMeshPrimitive::render(const bool renderFaces, const bool renderEdges) const
{
    if (m_vertexArrayObjectIDForFaces == 0)
        throw new Exception("BackendMesh: invalid mesh cache");

    if (renderFaces)
    {
        const auto& faceShader = m_scene.getMeshFaceShader();

        glBindVertexArray(m_vertexArrayObjectIDForFaces);
        glUseProgram(faceShader.m_shaderProgramID);

        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_CULL_FACE);
//        glEnable(GL_POLYGON_OFFSET_FILL);
//        glPolygonOffset(1.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, 0);
    }
    if (renderEdges)
    {
        const auto& edgeShader = m_scene.getMeshEdgeShader();

        glUseProgram(edgeShader.m_shaderProgramID);

        glLineWidth(2.0f);
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
    }
}
void OpenGLMeshPrimitive::insertMesh(const Mesh& mesh)
{
    const int numVertices = mesh.numVertices();
    assert(mesh.m_normals.size() == numVertices);
    assert(mesh.m_textureCoords.size() == numVertices);

    const auto vertexArrayObjectIDForFaces = glsGenAndBindVertexArrayObject();

    const auto vertexBufferID  = glsCreateAndAttachBufferToAttribute(vertexArrayObjectIDForFaces, 0, mesh.m_vertices);
    const auto normalBufferID  = glsCreateAndAttachBufferToAttribute(vertexArrayObjectIDForFaces, 1, mesh.m_normals);
    const auto uvcoordBufferID = glsCreateAndAttachBufferToAttribute(vertexArrayObjectIDForFaces, 2, mesh.m_textureCoords);

    {//Faces
        const auto& faces = mesh.m_faces;
        const int numFaces = mesh.numFaces();
        const auto faceSize = sizeof(faces[0]);

        const auto indexBufferID = glsCreateBuffer();
        glNamedBufferStorage(indexBufferID, numFaces * faceSize, faces.data(), 0);
        glVertexArrayElementBuffer(vertexArrayObjectIDForFaces, indexBufferID);

        m_vertexArrayObjectIDForFaces = vertexArrayObjectIDForFaces;
    }
    {//Edges
        const auto& edges = mesh.m_edges;
        const auto numEdges = mesh.numEdges();
        const auto vertexArrayObjectIDForEdges = glsGenAndBindVertexArrayObject();
        glsAttachBufferToAttribute<3>(vertexArrayObjectIDForEdges, vertexBufferID, 0);
        vector<array<int,2>> edgeVertexIndices; edgeVertexIndices.reserve(numEdges);
        for (const auto& e : mesh.m_edges)
            edgeVertexIndices.emplace_back(e.m_vis);

        const auto edgeSize = sizeof(edgeVertexIndices[0]);
        
        const auto indexBufferID = glsCreateBuffer();
        glNamedBufferStorage(indexBufferID, numEdges * edgeSize, edgeVertexIndices.data(), 0);
        glVertexArrayElementBuffer(vertexArrayObjectIDForEdges, indexBufferID);

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
