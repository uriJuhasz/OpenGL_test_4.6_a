#include "OpenGLBackend.h"

#include "private/OpenGLContext.h"

#include <memory>
using namespace std;

static unique_ptr<OpenGLContext> currentContext;

BackendContext& OpenGLBackend::getContext()
{
    if (!currentContext)
        currentContext = unique_ptr<OpenGLContext>(OpenGLContext::make());
    return *currentContext;
}
   
