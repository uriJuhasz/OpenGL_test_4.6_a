//#include "Math.h"
#include "Geometry/Mesh.h"

#include "IO/MeshLoader.h"

#include "Scene/Scene.h"

#include "OpenGLBackend/OpenGLUtilities.h"

#include "Backend/BackendWindow.h"
#include "View/View.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

#include <GLFW/glfw3.h>

using namespace std;

float toFloat(const double d) { return static_cast<float>(d); }


class GLFWContext;
static GLFWContext* currentContext = nullptr;

class GLFWWindow;
class GLFWContext
{
public:
    GLFWContext()
    {
        if (currentContext)
            return;
        m_valid = glfwInit();
        if (m_valid)
        {
            currentContext = this;
            glfwSetErrorCallback(glfwErrorCallback);
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

    operator bool() const { return m_valid; }

private:
    static void glfwErrorCallback(int error, const char* description)
    {
        cerr << " GLFW error: " << description << endl;
    }

private:
    bool m_valid = false;
//    vector<GLFWWindow*> windows
};

class GLFWWindow final : public BackendWindow
{
public:
    explicit GLFWWindow(const GLFWContext& context, const std::string& title) : m_context(context)
    {
        if (!context)
            return;
        if (const auto window = glfwCreateWindow(c_defaultWidth, c_defaultHeight, title.c_str(), nullptr, nullptr))
        {
            m_window = window;
            s_allWindows.push_back({ m_window,this });

            glfwMakeContextCurrent(window);
            glfwMaximizeWindow(window);
            glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
            glfwSetScrollCallback(window, glfwScrollCallback);
            glfwSetCursorPosCallback(window, glfwMousePosCallback);
            glfwSetKeyCallback(window, glfwKeyCallback);

            if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }

    ~GLFWWindow()
    {
        if (m_window)
        {
            for (int i = 0; i < s_allWindows.size(); ++i) //Remove from list
            {
                if (s_allWindows[i].first == m_window)
                {
                    s_allWindows[i] = s_allWindows.back();
                    s_allWindows.resize(s_allWindows.size() - 1);
                }
            }
            glfwDestroyWindow(m_window);
        }
    }

public:
    array<int, 2> getFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return { width,height };
    }
    operator bool() const { return m_window != nullptr; }

public:
    void registerView(ViewInterface* view) override
    {
        m_view = view;
    }

public:
    void eventLoop()
    {
        if (!m_view)
            return;
        m_view->setupScene();

        int frameIndex = 0;

        while (!glfwWindowShouldClose(m_window))
        {
            if (!updateNeeded())
            {
                glfwWaitEvents();
                continue;
            }

            m_view->renderScene();

            // put the stuff we've been drawing onto the display
            glfwSwapBuffers(m_window);

            checkGLErrors();

            frameIndex++;
            updateFinished();

            // update other events like input handling 
            glfwPollEvents();
        }
    }

private:
    const GLFWContext& m_context;
    GLFWwindow* m_window = nullptr;
    static const int c_defaultWidth  = 800;
    static const int c_defaultHeight = 800;

    ViewInterface* m_view;

public:
    bool isLeftMouseButtonPressed() const
    {
        return glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    }
    bool isMiddleMouseButtonPressed() const
    {
        return glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    }
    bool isRightMouseButtonPressed() const
    {
        return glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    }


private:
    Vector2 m_oldMousePos;
    bool m_oldMousePosValid = false;
    void mouseMoveCallback(const Vector2& newPos)
    {
        if (const auto view = m_view)
        {
            if (m_oldMousePosValid)
            {
                const auto delta = newPos - m_oldMousePos;
                m_view->mouseMoveCallback(delta, m_oldMousePos, newPos);
            }
        }
        m_oldMousePos = newPos;
        m_oldMousePosValid = true;
    }
    
    void mouseWheelCallback(const Vector2& delta)
    {
        if (const auto view = m_view)
            view->mouseWheelCallback(delta);
    }
    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void glfwMousePosCallback(GLFWwindow* window, double xpos, double ypos);

    static void glfwWindowResizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    bool m_needUpdate = true;
    void requestUpdate() override
    {
        m_needUpdate = true;
    }
    bool updateNeeded()
    {
        return m_needUpdate;
    }

    void updateFinished()
    {
        m_needUpdate = false;
    }

private:
    static vector<pair<const GLFWwindow*, GLFWWindow*>> s_allWindows;
    static GLFWWindow* findWindow(const GLFWwindow* wtf)
    {
        for (const auto& p : s_allWindows)
            if (p.first == wtf)
                return p.second;
        return nullptr;
    }
};

vector<pair<const GLFWwindow*, GLFWWindow*>> GLFWWindow::s_allWindows = {};

int main()
{
    cout << "Start" << endl;
    cout << endl;
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\90-3ds\3ds\Dragon 2.5_3ds.3ds)";
    const auto fileName = R"(C:\Users\rossd\Downloads\3d\Cat_v1_L2.123c6a1c5523-ac23-407e-9fbb-d0649ffb5bcb\12161_Cat_v1_L2.obj)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\Scorpio N05808.3ds)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\fc6bdb2aea4b58c23a3e8d4e87fba763\Elephant N090813.3DS)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\a8cfcfd0082c61bad7aa4fbd1c57a277\Ship hms victory frigate nelson N270214.3DS)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\e6eadc4ff882b84784dd133168c1f099\Autogenerator BelMag N170211.3DS)";
    cout << " Loading mesh: " << fileName;
    auto meshPtr = MeshLoader::loadMesh(fileName);
    if (meshPtr)
    {
        
        if (GLFWContext context = GLFWContext())
        {
            
            if (GLFWWindow window = GLFWWindow(context, "OpenGLTest"))
            {
                const auto view = View::makeView(window);
                view->setMesh(move(meshPtr));
                window.eventLoop();
            }
            else
            {
                cerr << " GLFW: failed to create window" << endl;
            }
        }
        else
        {
            cerr << " GLFW initialization failed, aborting" << endl;
        }
    }
    else
    {
        cerr << " Could not load mesh, aborting" << endl;
    }

    cout << "end" << endl;
}

void GLFWWindow::glfwScrollCallback(GLFWwindow* glfwWindow, double dxoffsetD, double yoffsetD)
{
    if (const auto windowPtr = findWindow(glfwWindow))
        windowPtr->mouseWheelCallback(Vector2(toFloat(dxoffsetD), toFloat(yoffsetD)));
}
void GLFWWindow::glfwMousePosCallback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
    const Vector2 newPos(toFloat(xpos), toFloat(ypos));
    if (const auto windowPtr = findWindow(glfwWindow))
        windowPtr->mouseMoveCallback(newPos);
}
void GLFWWindow::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

