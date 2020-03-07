#pragma once

#include "Math/Matrix.h"
#include "Math/Vector.h"

#include <string>

class BackendShaderProgram
{
protected:
	BackendShaderProgram() {}
	BackendShaderProgram(const BackendShaderProgram&) = delete;
	BackendShaderProgram(BackendShaderProgram&&) = delete;
	BackendShaderProgram& operator=(const BackendShaderProgram&) = delete;
	BackendShaderProgram& operator=(BackendShaderProgram&&) = delete;

public:
	virtual void setParameter(const std::string& name, const float value) = 0;
	virtual void setParameter(const std::string& name, const Vector3& value) = 0;
	virtual void setParameter(const std::string& name, const Vector4& value) = 0;
	virtual void setParameter(const std::string& name, const Matrix4x4 value) = 0;
	virtual void setParameter(const std::string& name, const int value) = 0;

public:
	virtual ~BackendShaderProgram() {}
};

