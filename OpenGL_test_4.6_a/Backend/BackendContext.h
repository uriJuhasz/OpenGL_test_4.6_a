#pragma once
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

