#include "OpenGLShaderProgram.h"
#include "OpenGLBackend/OpenGLUtilities.h"

#include <string>
using std::to_string;
using std::string;

bool OpenGLShaderProgram::hasLight(const int lightIndex) const
{
	return glGetUniformLocation(m_shaderProgramID, ("light" + to_string(lightIndex) + "Position").c_str());
}

void OpenGLShaderProgram::setParameter(const std::string& name, const float value)
{
	glUseProgram(m_shaderProgramID);
	glUniform1f(glGetUniformLocation(m_shaderProgramID, name.c_str()), value);
	glsCheckErrors();
}
void OpenGLShaderProgram::setParameter(const std::string& name, const Vector3& value)
{
	glUseProgram(m_shaderProgramID);
	glUniform3fv(glGetUniformLocation(m_shaderProgramID, name.c_str()), 1, value.data());
	glsCheckErrors();
}
void OpenGLShaderProgram::setParameter(const std::string& name, const Vector4& value)
{
	glUseProgram(m_shaderProgramID);
	glUniform4fv(glGetUniformLocation(m_shaderProgramID, name.c_str()), 1, value.data());
	glsCheckErrors();
}
void OpenGLShaderProgram::setParameter(const std::string& name, const Matrix4x4 value)
{
	glUseProgram(m_shaderProgramID);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgramID, name.c_str()),1, true, value.data());
	glsCheckErrors();
}
void OpenGLShaderProgram::setParameter(const std::string& name, const int value)
{
	glUseProgram(m_shaderProgramID);
	glUniform1i(glGetUniformLocation(m_shaderProgramID, name.c_str()), value);
	glsCheckErrors();
}
