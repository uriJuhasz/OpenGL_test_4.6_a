#pragma once
#include "ViewInterface.h"

#include <array>
class BackendWindow
{
protected:
    BackendWindow() {}

public:
    virtual ~BackendWindow() {}
public:
	virtual void registerView(ViewInterface* view) = 0;

    virtual std::array<int, 2> getFramebufferSize() const = 0;
    virtual bool isLeftMouseButtonPressed() const = 0;
    virtual bool isMiddleMouseButtonPressed() const = 0;
    virtual bool isRightMouseButtonPressed() const = 0;

    virtual void requestUpdate() = 0;
};

