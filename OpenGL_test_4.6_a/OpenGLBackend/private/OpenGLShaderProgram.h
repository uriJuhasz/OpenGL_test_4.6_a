#pragma once

#pragma warning (disable:4250)

#include "Backend/BackendShaderProgram.h"
#include "Backend/BackendStandardShaderProgram.h"
#include "Backend/BackendTesselationShaderProgram.h"


#include <GL/glew.h>

class OpenGLShaderProgram : public virtual BackendShaderProgram
{
public:
	explicit OpenGLShaderProgram(GLuint shaderProgramID) : m_shaderProgramID(shaderProgramID) {}
public:
	void setParameter(const std::string& name, const float value) override;
	void setParameter(const std::string& name, const Vector3& value) override;
	void setParameter(const std::string& name, const Vector4& value) override;
	void setParameter(const std::string& name, const Matrix4x4 value) override;

	void setParameter(const std::string& name, const int value) override;

private:
	GLuint m_shaderProgramID;
};

class OpenGLStandardShaderProgram
	: public OpenGLShaderProgram
	, public virtual BackendStandardShaderProgram
{
public:
	using OpenGLShaderProgram::OpenGLShaderProgram;
};

class OpenGLTessellationShaderProgram
	: public OpenGLShaderProgram
	, public virtual BackendTesselationShaderProgram
{
public:
	using OpenGLShaderProgram::OpenGLShaderProgram;
};
