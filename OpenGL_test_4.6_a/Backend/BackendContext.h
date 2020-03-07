#pragma once
class BackendWindow;

class BackendContext
{
public:
	virtual BackendWindow* createWindow() = 0;
};

