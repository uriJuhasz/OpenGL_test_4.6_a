#include "OpenGLBezierPatchInstance.h"

#include "OpenGLBackend/private/OpenGLUtilities.h"
#include "Utilities/Misc.h"

using std::vector;
using std::array;

OpenGLBezierPatchInstance::OpenGLBezierPatchInstance(const OpenGLBezierPatchPrimitive& primitive)
	: OpenGLGraphicObject(primitive.getScene())
	, OpenGLSurface(primitive.getScene())
	, m_patchPrimitive(primitive)
{
}

OpenGLBezierPatchInstance& OpenGLBezierPatchInstance::createInstance() const
{
	return getScene().makeInstance(*this);
}

void OpenGLBezierPatchInstance::renderMain()
{
	if (m_facesVisible)
	{
		auto& faceShader = m_patchPrimitive.getFaceShader();

		faceShader.setParameter("modelMatrix", m_modelMatrix);
	}
	if (m_edgesVisible)
	{
		auto& edgeShader = m_patchPrimitive.getEdgeShader();

		edgeShader.setParameter("modelMatrix", m_modelMatrix);
		edgeShader.setParameter("edgeColor", m_edgesColor.m_value);

		glLineWidth(m_edgesWidth);
	}
	m_patchPrimitive.render(m_facesVisible,m_edgesVisible);

	if (m_controlVisible)
	{
		constexpr int c_numVerticesPerPatch = 16;
		{ //Control points
			const auto vao = m_patchPrimitive.getVertexArrayObject();
			glBindVertexArray(vao);
			//		const auto vertexBuffer = glsGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, 0);
			constexpr float c_controlPointSize = 16.0f;
			glPointSize(c_controlPointSize);
			auto& shader = getScene().getPointsShader();
			shader.setParameter("modelMatrix", m_modelMatrix);
			shader.setParameter("fixedFragmentColor", ColorRGBA::Red);
			glUseProgram(shader.m_shaderProgramID);
			glDrawArrays(GL_POINTS, 0, c_numVerticesPerPatch);
			glUseProgram(0);
		}
		{ //Control grid
			const auto pvao = m_patchPrimitive.getVertexArrayObject();
			glBindVertexArray(pvao);
			const auto vertexBuffer = glsGetVertexAttribInt(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, 0);

			const auto vertexArrayObject = glsGenAndBindVertexArrayObject();
			{
				glsAttachBufferToAttribute<3>(vertexArrayObject, vertexBuffer, 0);
			}
			const auto indexBufferID = glsCreateBuffer();
			int numEdges = 0;
			{
				vector<array<int, 2>> edgeBuffer; edgeBuffer.reserve(c_numVerticesPerPatch * 2);
				for (int i = 0; i < 4; ++i)
					for (int j = 0; j < 4; ++j)
					{
						if (j < 3)
							edgeBuffer.emplace_back(array<int, 2>{i * 4 + j,  i      * 4 + j + 1 });
						if (i < 3)
							edgeBuffer.emplace_back(array<int, 2>{i * 4 + j, (i + 1) * 4 + j     });
					}

				numEdges = toInt(edgeBuffer.size());
				const auto edgeSize = sizeof(edgeBuffer[0]);
				glNamedBufferStorage(indexBufferID, numEdges * edgeSize, edgeBuffer.data(), 0);
				glVertexArrayElementBuffer(vertexArrayObject, indexBufferID);
			}

			{
				auto& shader = getScene().getMeshEdgeShader();
				shader.setParameter("modelMatrix", m_modelMatrix);
				shader.setParameter("fixedFragmentColor", ColorRGBA::Yellow);
				glUseProgram(shader.m_shaderProgramID);
				glDrawElements(GL_LINES, numEdges * 2, GL_UNSIGNED_INT, 0);
				glUseProgram(0);			
			}
			{
				glDeleteBuffers(1, &indexBufferID);
				glDeleteVertexArrays(1, &vertexArrayObject);
			}
		}

	}
}

OpenGLGraphicObject::BoundingBox OpenGLBezierPatchInstance::getBoundingBox() const
{
	return calculateBoundingBox(toVector(m_patchPrimitive.getPatch().getVertices()));
}

const OpenGLBezierPatchPrimitive& OpenGLBezierPatchInstance::getPrimitive() const
{
	return m_patchPrimitive;
}
