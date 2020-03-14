#include "OpenGLMesh.h"
#include "OpenGLBackend/OpenGLUtilities.h"

#include "Utilities/Exception.h"
#include "Utilities/Misc.h"

#include <cassert>
#include <iostream>

using std::vector;
using std::array;

static int glGetVertexAttribInt(const GLenum type, const int attributeIndex)
{
    GLint val;
    glGetVertexAttribiv(attributeIndex, type, &val);
    return val;
}

GLuint insertMesh(const Mesh& mesh);

OpenGLMesh::OpenGLMesh(const Mesh& mesh)
    : m_numFaces(mesh.numFaces())
    , m_numEdges(mesh.numEdges())
{
    insertMesh(mesh);
}

OpenGLMesh::~OpenGLMesh()
{
    constexpr int c_maxVertexAttributes = 3;
    if (m_vertexArrayObjectIDForFaces)
    {
        glBindVertexArray(m_vertexArrayObjectIDForFaces);
        for (int i = 0; i < c_maxVertexAttributes; ++i)
        {
            if (glGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_ENABLED, i))
            {
                GLuint bufferID = glGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, 2);
                glDeleteBuffers(1, &bufferID);
            }
            
        }
        GLuint vertexIndexBufferID = 0;
        glGetVertexArrayiv(m_vertexArrayObjectIDForFaces,GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&vertexIndexBufferID);
        std::cout << " Deleting vertex index buffer " << vertexIndexBufferID << std::endl;
        glDeleteBuffers(1,&vertexIndexBufferID);
    }
    glDeleteVertexArrays(1, &m_vertexArrayObjectIDForFaces);
}

void OpenGLMesh::setFaceShader(const BackendStandardShaderProgram* faceShader) { m_faceShader = dynamic_cast<const OpenGLStandardShaderProgram*>(faceShader); }
void OpenGLMesh::setEdgeShader(const BackendStandardShaderProgram* edgeShader) { m_edgeShader = dynamic_cast<const OpenGLStandardShaderProgram*>(edgeShader); }

void OpenGLMesh::render(const bool renderFaces, const bool renderEdges)
{
    if (m_vertexArrayObjectIDForFaces == 0)
        throw new Exception("BackendMesh: invalid mesh cache");

    if (renderFaces && m_faceShader)
    {
        glBindVertexArray(m_vertexArrayObjectIDForFaces);
        glUseProgram(m_faceShader->m_shaderProgramID);

        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, 0);
    }
    if (renderEdges && m_edgeShader)
    {
        glUseProgram(m_edgeShader->m_shaderProgramID);

        glLineWidth(2.0f);
        glDepthFunc(GL_LEQUAL);
        static bool aaLines = false;
        const bool antialiasedLines = aaLines;
        aaLines = !aaLines;
        if (antialiasedLines)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        if (renderFaces)
        {
            glPolygonMode(GL_FRONT, GL_LINE);
            glEnable(GL_CULL_FACE);
            if (antialiasedLines)
            {
                glEnable(GL_POLYGON_SMOOTH);
                glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
            }
            else
            {
                glDisable(GL_POLYGON_SMOOTH);
            }

            glBindVertexArray(m_vertexArrayObjectIDForFaces);
            glDrawElements(GL_TRIANGLES, m_numFaces*3, GL_UNSIGNED_INT, 0);

        }
        else
        {
            if (antialiasedLines)
            {
                glEnable(GL_LINE_SMOOTH);
                glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            }
            else
            {
                glEnable(GL_LINE_SMOOTH);
            }
            glBindVertexArray(m_vertexArrayObjectIDForEdges);
            glDrawElements(GL_LINES, m_numEdges * 2, GL_UNSIGNED_INT, 0);
        }
    }
}

GLuint glsCreateBuffer()
{
    GLuint bufferID = 0;
    glCreateBuffers(1, &bufferID);
    return bufferID;
}
template<unsigned int D>static GLuint glsCreateBuffer(
    const vector<Vector<D>>& values
)
{
    const auto valueSize = sizeof(values[0]); // sizeof(values[0]);
    const auto numValues = toInt(values.size());

    const auto bufferID = glsCreateBuffer();
    //    std::cout << "  Created buffer " << bufferID << " for attribute " << attributeIndex << std::endl;
    glNamedBufferStorage(bufferID, numValues * valueSize, values.data(), 0);
    return bufferID;
}

template<unsigned int D>static void glsAttachBufferToAttribute(
    const GLuint vertexArrayObjectID,
    const GLuint bufferID,
    const int attributeIndex
    )
{
    glVertexArrayVertexBuffer(vertexArrayObjectID, attributeIndex, bufferID, 0, D*sizeof(float));
    glEnableVertexArrayAttrib(vertexArrayObjectID, attributeIndex);
    glVertexArrayAttribFormat(vertexArrayObjectID, attributeIndex, D, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vertexArrayObjectID, attributeIndex, attributeIndex);
}

template<unsigned int D>static GLuint glsCreateAndAttachBufferToAttribute(
    const GLuint vertexArrayObjectID,
    const int attributeIndex,
    const vector<Vector<D>>& values
)
{
    const auto bufferID = glsCreateBuffer(values);
    glsAttachBufferToAttribute<D>(vertexArrayObjectID, bufferID, attributeIndex);

    return bufferID;
}

void OpenGLMesh::insertMesh(const Mesh& mesh)
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
