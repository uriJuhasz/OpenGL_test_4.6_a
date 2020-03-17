#pragma once

#include "BackendWindow.h"

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
};

