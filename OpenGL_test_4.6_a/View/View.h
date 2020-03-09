#pragma once

#include "Backend/BackendViewInterface.h"
#include "Backend/BackendWindow.h"

#include "Geometry/Mesh.h"

#include <memory>

class View : public BackendViewInterface
{
public:
    static View* makeView(BackendWindow& backendWindow);
    
    virtual void setMesh(std::unique_ptr<Mesh> m) = 0;
};

