#pragma once

#pragma warning (disable:4250)

#include "Math/Vector.h"
#include "Math/Matrix.h"

#include <GL/glew.h>

#include <string>
class OpenGLShaderProgram
{
public:
	explicit OpenGLShaderProgram(GLuint shaderProgramID) : m_shaderProgramID(shaderProgramID) {}

public:
	bool hasLight(const int) const;

public:
	void setParameter(const std::string& name, const float value);
	void setParameter(const std::string& name, const Vector3& value);
	void setParameter(const std::string& name, const Vector4& value);
	void setParameter(const std::string& name, const Matrix4x4 value);

	void setParameter(const std::string& name, const int value);

	GLuint m_shaderProgramID;
private:
};

class OpenGLStandardShaderProgram
	: public OpenGLShaderProgram
{
public:
	using OpenGLShaderProgram::OpenGLShaderProgram;
};

class OpenGLTessellationShaderProgram
	: public OpenGLShaderProgram
{
public:
	using OpenGLShaderProgram::OpenGLShaderProgram;
};
