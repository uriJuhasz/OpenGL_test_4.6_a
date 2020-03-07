#pragma once

#include "Backend/ViewInterface.h"
#include "Backend/BackendWindow.h"

#include "Geometry/Mesh.h"

#include <memory>

class View : public ViewInterface
{
public:
    static View* makeView(BackendWindow& backendWindow);
    
    virtual void setMesh(std::unique_ptr<Mesh> m) = 0;
};

