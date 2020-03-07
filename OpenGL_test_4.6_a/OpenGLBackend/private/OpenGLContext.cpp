#include "OpenGLContext.h"

#include "OpenGLWindow.h"

#include "Utilities/Exception.h"

#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

class GLFWContext;

class GLFWWindow;
class GLFWContext final : public OpenGLContext
{
private:
    GLFWContext(const GLFWContext&) = delete;
    GLFWContext(GLFWContext&&) = delete;
    GLFWContext& operator=(const GLFWContext&) = delete;
    GLFWContext& operator=(const GLFWContext&&) = delete;
public:
    GLFWContext()
    {
        m_valid = glfwInit();
        if (m_valid)
        {
            glfwSetErrorCallback(glfwErrorCallback);
        }
        else
        {
            const char* errorMessageC;
            glfwGetError(&errorMessageC);
            const auto errorMessage = errorMessageC ? errorMessageC : "Unknown error";
            throw new Exception(string("Could not initialize GLFW : ") + errorMessage);
        }
    }

    ~GLFWContext()
    {
        if (m_valid)
        {
            glfwTerminate();
            m_valid = false;
        }
    }

    operator bool() const override { return m_valid; }

public:
    BackendWindow* createWindow() override
    {
        return OpenGLWindow::make(*this);
    }

private:
    static void glfwErrorCallback(int error, const char* description)
    {
        cerr << " GLFW error: " << description << endl;
    }

private:
    bool m_valid = false;
};

OpenGLContext* OpenGLContext::make()
{
    return new GLFWContext();
}
