#pragma once

#include "BackendWindow.h"
#include "BackendStandardShaderProgram.h"
#include "BackendTesselationShaderProgram.h"

#include <memory>
#include <string>

class BackendWindow;
class BackendContext
{
protected:
	BackendContext() {}
public:
	virtual ~BackendContext() {}

	virtual operator bool() const = 0;

	virtual BackendWindow* createWindow() = 0;

public:
    virtual void setShaderBasePath(const std::string& path) = 0;
    virtual std::unique_ptr<BackendStandardShaderProgram> makeStandardShaderProgram(
        const std::string& fileName,
        const std::string& title) = 0;
    virtual std::unique_ptr<BackendTesselationShaderProgram> makeTessellationShaderProgram(
        const std::string& fileName,
        const std::string& title) = 0;

public:
    virtual int getMaxTessellationLevel() const = 0;
};

