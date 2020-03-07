#pragma once

#include "ViewInterface.h"
#include "BackendContext.h"

#include <array>

class BackendContext;

class BackendWindow
{
protected:
    BackendWindow() {}

public:
    virtual ~BackendWindow() {}

public:
    virtual void init() = 0;

    virtual operator bool() const = 0;

    virtual BackendContext& getContext() = 0;
    
public:
    virtual void registerView(ViewInterface* view) = 0;

public:
    virtual std::array<int, 2> getFramebufferSize() const = 0;
    virtual Vector2 getViewportDimensions() const = 0;

public:
    virtual void eventLoop() = 0;
    virtual bool isLeftMouseButtonPressed() const = 0;
    virtual bool isMiddleMouseButtonPressed() const = 0;
    virtual bool isRightMouseButtonPressed() const = 0;

public:
    virtual void requestUpdate() = 0;

public:

};

