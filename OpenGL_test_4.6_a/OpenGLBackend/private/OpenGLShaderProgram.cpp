#include "OpenGLShaderProgram.h"
#include "OpenGLUtilities.h"

#include <string>
using std::to_string;
using std::string;

bool OpenGLShaderProgram::hasLight(const int lightIndex) const
{
	return glGetUniformLocation(m_shaderProgramID, ("light" + to_string(lightIndex) + "Position").c_str());
}

template<class F, class ...Args> inline void setParameterT(const GLuint programID, const std::string& name, F f, Args... args)
{
	const auto location = glGetUniformLocation(programID, name.c_str());
	f(programID, location, args...);
	glsCheckErrors();
}

void OpenGLShaderProgram::setParameter(const std::string& name, const float value)
{
	setParameterT(m_shaderProgramID, name, glProgramUniform1f, value);
}
void OpenGLShaderProgram::setParameter(const std::string& name, const Vector3& value)
{
	setParameterT(m_shaderProgramID, name, glProgramUniform3fv, 1, value.data());
}
void OpenGLShaderProgram::setParameter(const std::string& name, const Vector4& value)
{
	setParameterT(m_shaderProgramID, name, glProgramUniform4fv, 1, value.data());
}
void OpenGLShaderProgram::setParameter(const std::string& name, const Matrix4x4& value)
{
	setParameterT(m_shaderProgramID, name, glProgramUniformMatrix4fv, 1, true, value.data());
}
void OpenGLShaderProgram::setParameter(const std::string& name, const int value)
{
	setParameterT(m_shaderProgramID, name, glProgramUniform1i, value);
}

template<class F, class ...Args> inline void setParameterIfExistsT(const GLuint programID, const std::string& name, F f, Args... args)
{
	const auto location = glGetUniformLocation(programID, name.c_str());
	if (location != -1)
	{
		f(programID, location, args...);
	}
	glsCheckErrors();
}

void OpenGLShaderProgram::setParameterIfExists(const std::string& name, const float value)
{
	setParameterIfExistsT(m_shaderProgramID, name, glProgramUniform1f, value);
}
void OpenGLShaderProgram::setParameterIfExists(const std::string& name, const Vector2& value)
{
	setParameterIfExistsT(m_shaderProgramID, name, glProgramUniform2fv, 1, value.data());
}
void OpenGLShaderProgram::setParameterIfExists(const std::string& name, const Vector3& value)
{
	setParameterIfExistsT(m_shaderProgramID, name, glProgramUniform3fv, 1, value.data());
}
void OpenGLShaderProgram::setParameterIfExists(const std::string& name, const Vector4& value)
{
	setParameterIfExistsT(m_shaderProgramID, name, glProgramUniform4fv, 1, value.data());
}

void OpenGLShaderProgram::setParameterIfExists(const std::string& name, const Matrix4x4& value)
{
	setParameterIfExistsT(m_shaderProgramID, name, glProgramUniformMatrix4fv, 1, true, value.data());
}
void OpenGLShaderProgram::setParameterIfExists(const std::string& name, const int value)
{
	setParameterIfExistsT(m_shaderProgramID, name, glProgramUniform1i, value);
}
