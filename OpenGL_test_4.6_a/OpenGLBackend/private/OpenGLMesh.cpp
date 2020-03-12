#include "OpenGLMesh.h"
#include "OpenGLBackend/OpenGLUtilities.h"

#include "Utilities/Exception.h"
#include "Utilities/Misc.h"

#include <cassert>
#include <iostream>

using std::vector;

static int glGetVertexAttribInt(const GLenum type, const int attributeIndex)
{
    GLint val;
    glGetVertexAttribiv(attributeIndex, type, &val);
    return val;
}

GLuint insertMesh(const Mesh& mesh);

OpenGLMesh::OpenGLMesh(const Mesh& mesh)
    : m_numFaces(mesh.numFaces())
{
    insertMesh(mesh);
}

OpenGLMesh::~OpenGLMesh()
{
    constexpr int c_maxVertexAttributes = 3;
    if (m_vertexArrayObjectID)
    {
        glBindVertexArray(m_vertexArrayObjectID);
        for (int i = 0; i < c_maxVertexAttributes; ++i)
        {
            if (glGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_ENABLED, i))
            {
                GLuint bufferID = glGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, 2);
                glDeleteBuffers(1, &bufferID);
            }
            
        }
        GLuint vertexIndexBufferID = 0;
        glGetVertexArrayiv(m_vertexArrayObjectID,GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&vertexIndexBufferID);
        std::cout << " Deleting vertex index buffer " << vertexIndexBufferID << std::endl;
        glDeleteBuffers(1,&vertexIndexBufferID);
    }
    glDeleteVertexArrays(1, &m_vertexArrayObjectID);
}

void OpenGLMesh::setFaceShader(const BackendStandardShaderProgram* faceShader) { m_faceShader = dynamic_cast<const OpenGLStandardShaderProgram*>(faceShader); }
void OpenGLMesh::setEdgeShader(const BackendStandardShaderProgram* edgeShader) { m_edgeShader = dynamic_cast<const OpenGLStandardShaderProgram*>(edgeShader); }

void OpenGLMesh::render(const bool renderFaces, const bool renderEdges)
{
    if (m_vertexArrayObjectID == 0)
        throw new Exception("BackendMesh: invalid mesh cache");
    glBindVertexArray(m_vertexArrayObjectID);

    if (renderFaces && m_faceShader)
    {
        glUseProgram(m_faceShader->m_shaderProgramID);

        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, 0);
    }
    if (renderEdges && m_edgeShader)
    {
        glLineWidth(1.0f);
        //        glEnable(GL_BLEND);
        //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //        glEnable(GL_LINE_SMOOTH);
        //        glEnable(GL_POLYGON_SMOOTH);
        //        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        //        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        //        glDisable(GL_CULL_FACE);

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0, 0.0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(m_edgeShader->m_shaderProgramID);

        glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, 0);
    }
}

template<unsigned int D>static GLuint attachBufferToAttribute(
    const GLuint vertexArrayObjectID,
    const int attributeIndex,
    const vector<Vector<D>>& values
)
{
    const auto valueSize = sizeof(values[0]);
    const auto numValues = toInt(values.size());

    GLuint bufferID = 0;
    glCreateBuffers(1, &bufferID);
//    std::cout << "  Created buffer " << bufferID << " for attribute " << attributeIndex << std::endl;
    glNamedBufferStorage(bufferID, numValues * valueSize, values.data(), 0);
    glVertexArrayVertexBuffer(vertexArrayObjectID, attributeIndex, bufferID, 0, valueSize);
    glEnableVertexArrayAttrib(vertexArrayObjectID, attributeIndex);
    glVertexArrayAttribFormat(vertexArrayObjectID, attributeIndex, D, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vertexArrayObjectID, attributeIndex, attributeIndex);

    return bufferID;
}

void OpenGLMesh::insertMesh(const Mesh& mesh)
{
    const int numVertices = mesh.numVertices();

    const auto vertexArrayObjectID = glsGenAndBindVertexArrayObject();

    assert(mesh.m_normals.size() == numVertices);
    assert(mesh.m_textureCoords.size() == numVertices);

    const auto vertexBufferID = attachBufferToAttribute(vertexArrayObjectID,0,mesh.m_vertices);
    const auto normalBufferID = attachBufferToAttribute(vertexArrayObjectID, 1, mesh.m_normals);
    const auto uvcoordBufferID = attachBufferToAttribute(vertexArrayObjectID, 2, mesh.m_textureCoords);

    {//Faces
        const auto& faces = mesh.m_faces;
        const int numFaces = mesh.numFaces();
        const auto faceSize = sizeof(faces[0]);
        GLuint indexBufferID;
        glCreateBuffers(1, &indexBufferID);
        glNamedBufferStorage(indexBufferID, numFaces * faceSize, faces.data(), GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(vertexArrayObjectID, indexBufferID);
    }

    m_vertexArrayObjectID = vertexArrayObjectID;
}
