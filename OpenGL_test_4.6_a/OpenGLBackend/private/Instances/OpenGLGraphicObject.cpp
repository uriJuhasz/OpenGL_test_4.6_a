#include "OpenGLGraphicObject.h"

#include "OpenGLBackend/private/OpenGLUtilities.h"

#include "Utilities/Misc.h"

using std::vector;
using std::array;
using std::min;
using std::max;

OpenGLGraphicObject::OpenGLGraphicObject(OpenGLScene& scene)
	: m_scene(scene)
{
}

void OpenGLGraphicObject::setVisibility(const bool isVisible)
{
	m_isVisible = isVisible;
}

bool OpenGLGraphicObject::isVisible() const
{
	return m_isVisible;
}

Matrix4x4 OpenGLGraphicObject::getModelMatrix() const
{
	return m_modelMatrix;
}

void OpenGLGraphicObject::render()
{
	if (isVisible())
	{
		if (m_boundingBoxVisibility)
			renderBoundingBox();
		renderMain();
	}
}

void OpenGLGraphicObject::renderBoundingBox()
{
	auto& shader = m_scene.getBoundingBoxShader();
	glUseProgram(shader.m_shaderProgramID);

	shader.setParameter("edgeColor", Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	const auto boundingBox = getBoundingBox().get();
	{
		const auto vertexArrayObject = glsGenAndBindVertexArrayObject();
		const vector<Vector3> vertices = { boundingBox[0],boundingBox[1] };
		glsCreateAndAttachBufferToAttribute(vertexArrayObject, 0, vertices);
		glUseProgram(shader.m_shaderProgramID);
		glDrawArrays(GL_LINES, 0, 2);
		glUseProgram(0);
		glsDeleteVertexArrayObjectAndAllBuffers(vertexArrayObject,1);
	}
}

void OpenGLGraphicObject::setModelMatrix(const Matrix4x4& newModelMatrix)
{
	m_modelMatrix = newModelMatrix;
}

void OpenGLGraphicObject::setBoundingBoxVisibility(const bool boundingBoxVisibility)
{
	m_boundingBoxVisibility = boundingBoxVisibility;
}

OpenGLGraphicObject::BoundingBox OpenGLGraphicObject::calculateBoundingBox(const vector<Vector3>& vertices) const
{
	const auto numVertices = toInt(vertices.size());

	const auto modelMatrix = getModelMatrix();
	array<Vector3,2> bb{ Vector3(),Vector3() };
	if (numVertices > 0)
	{
		const auto v0 = mulHomogeneous(modelMatrix, vertices[0]);
		bb = { v0, v0 };
		for (int vi = 1; vi < numVertices; ++vi)
		{
			const auto v = mulHomogeneous(modelMatrix, vertices[vi]);
			for (int i = 0; i < 3; ++i)
			{
				bb[0][i] = min(bb[0][i], v[i]);
				bb[1][i] = max(bb[1][i], v[i]);
			}
		}
	}
	return BoundingBox(bb);
}
