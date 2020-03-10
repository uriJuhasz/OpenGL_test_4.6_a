#include "OpenGLMesh.h"
#include "OpenGLBackend/OpenGLUtilities.h"

#include "Utilities/Exception.h"
#include "Utilities/Misc.h"

#include <cassert>

GLuint insertMesh(const Mesh& mesh);

OpenGLMesh::OpenGLMesh(const Mesh& mesh)
    : m_numFaces(mesh.numFaces())
{
    insertMesh(mesh);
}

OpenGLMesh::~OpenGLMesh()
{
    glDeleteVertexArrays(1, &m_vertexArrayObjectID);
    for (const auto bufferId : m_buffers)
        glDeleteBuffers(1,&bufferId);
}

void OpenGLMesh::setFaceShader(const BackendStandardShaderProgram* faceShader) { m_faceShader = dynamic_cast<const OpenGLStandardShaderProgram*>(faceShader); }
void OpenGLMesh::setEdgeShader(const BackendStandardShaderProgram* edgeShader) { m_edgeShader = dynamic_cast<const OpenGLStandardShaderProgram*>(edgeShader); }

template<unsigned int D> GLuint OpenGLMesh::makeAndRegisterBuffer(const vector<Vector<D>>& vs, const int attributeIndex)
{
    const auto buffer = glsMakeBuffer(vs, attributeIndex);
    m_buffers.push_back(buffer);
    return buffer;
}

void OpenGLMesh::render(const bool renderFaces, const bool renderEdges)
{
    if (m_vertexArrayObjectID == 0)
        throw new Exception("BackendMesh: invalid mesh cache");
    if (renderFaces && m_faceShader)
    {
        glBindVertexArray(m_vertexArrayObjectID);
        glUseProgram(m_faceShader->m_shaderProgramID);

        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, m_numFaces * 3, GL_UNSIGNED_INT, 0);
    }
    if (renderEdges && m_edgeShader)
    {
        glBindVertexArray(m_vertexArrayObjectID);
        glUseProgram(m_edgeShader->m_shaderProgramID);

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

        glPolygonMode(GL_FRONT, GL_LINE);
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
    glNamedBufferStorage(bufferID, numValues * valueSize, values.data(), 0);// GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayVertexBuffer(vertexArrayObjectID, attributeIndex, bufferID, 0, valueSize);
    glEnableVertexArrayAttrib(vertexArrayObjectID, attributeIndex);
    glVertexArrayAttribFormat(vertexArrayObjectID, attributeIndex, D, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vertexArrayObjectID, attributeIndex, attributeIndex);

    return bufferID;
}
void OpenGLMesh::insertMesh(const Mesh& mesh)
{
    const int numVertices = mesh.numVertices();

    const auto vertexArrayObjectID = glsGenAndBindVertexArray();

    { //Vertex positions
/*        const auto& vertices = mesh.m_vertices;

        GLuint vertexBufferID = 0;
        {
            const auto& vs = vertices;
            const auto attributeIndex = 0;
            GLuint bufferID = 0;
            const int D = 3;
            const auto vertexSize = sizeof(vs[0]);
            const auto numVertices = toInt(vertices.size());
            glCreateBuffers(1, &bufferID);
            glNamedBufferStorage(bufferID, vs.size() * sizeof(vs[0]), vs.data(), GL_DYNAMIC_STORAGE_BIT);
            glVertexArrayVertexBuffer(vertexArrayObject, attributeIndex, bufferID, 0, vertexSize);
            glEnableVertexArrayAttrib(vertexArrayObject, attributeIndex);
            glVertexArrayAttribFormat(vertexArrayObject, attributeIndex, D, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vertexArrayObject, attributeIndex, attributeIndex);
            glsCheckErrors();

            vertexBufferID = bufferID;
        }
//        const auto vertexBuffer = glsMakeBuffer(vertices, 0);*/
        const auto vertexBufferID = attachBufferToAttribute(vertexArrayObjectID,0,mesh.m_vertices);
        m_buffers.push_back(vertexBufferID);
    }

    { //Vertex normals
        const auto& normals = mesh.m_normals;
        assert(normals.size() == numVertices);
        const auto normalBuffer = glsMakeBuffer(normals, 1);
        m_buffers.push_back(normalBuffer);
    }

    { //Vertex UV coords
        const auto& uvCoords = mesh.m_textureCoords;
        assert(uvCoords.size() == numVertices);
        const auto uvcoordBuffer = glsMakeBuffer(uvCoords, 2);
        m_buffers.push_back(uvcoordBuffer);
    }

    {//Faces
        const auto& faces = mesh.m_faces;
        const int numFaces = mesh.numFaces();
        const auto fao = glsGenAndBindBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * sizeof(faces[0]), faces.data(), GL_STATIC_DRAW);
        glsCheckErrors();
    }
    m_vertexArrayObjectID = vertexArrayObjectID;
}
