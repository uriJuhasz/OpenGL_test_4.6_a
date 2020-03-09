#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

#include "OpenGLWindow.h"
#include "OpenGLContext.h"
#include "OpenGLMesh.h"
#include "OpenGLPatch.h"

#include "OpenGLBackend/OpenGLUtilities.h"

#include "Utilities/Exception.h"
#include "Utilities/Misc.h"


#include <cassert>
#include <vector>
#include <iostream>

using namespace std;

class OpenGLWindowImpl final : public OpenGLWindow
{
public:
    explicit OpenGLWindowImpl(OpenGLContext& context, const std::string& title = "")
        : m_context(context)
        , m_view(nullptr)
    {
        if (!context)
            throw new Exception("GLFWWindow: context is invalid");
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
        else
        {
            const char* errorMessageC;
            glfwGetError(&errorMessageC);
            const auto errorMessage = string(errorMessageC ? errorMessageC : "Unknown error");
            throw new Exception("GLFWWindow: failed to create window : " + errorMessage);
        }
    }

    ~OpenGLWindowImpl()
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

    OpenGLContext& getContext() override { return m_context; }

public:
    array<int, 2> getFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return { width,height };
    }
    Vector2 getViewportDimensions() const
    {
        array<int, 4> viewport;
        glGetIntegerv(GL_VIEWPORT, viewport.data());
        const float vpw = static_cast<float>(viewport[2] - viewport[0]);
        const float vph = static_cast<float>(viewport[3] - viewport[1]);
        return Vector2(vpw, vph);
    }

    operator bool() const { return m_window != nullptr; }

public:
    void registerView(BackendViewInterface* view) override
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

            // wipe the drawing surface clear
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glsCheckErrors();

            m_view->renderScene();

            // put the stuff we've been drawing onto the display
            glfwSwapBuffers(m_window);

            glsCheckErrors();

            frameIndex++;
            updateFinished();

            // update other events like input handling 
            glfwPollEvents();
        }
    }

private:
    OpenGLContext& m_context;
    GLFWwindow* m_window = nullptr;
    static const int c_defaultWidth = 800;
    static const int c_defaultHeight = 800;

    BackendViewInterface* m_view;

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

public:
    void init() override
    {
        if (!m_window)
            return;
        int w, h;
        glfwGetWindowSize(m_window, &w, &h);
        glViewport(0, 0, w, h);

        // start GLEW extension handler
        glewExperimental = GL_TRUE;
        glewInit();

        //Look at GPUs
#ifdef _WIN32
        {
            if (WGLEW_NV_gpu_affinity)
            {
                for (UINT gpu = 0; true; ++gpu)
                {
                    HGPUNV hGPU = 0;
                    if (!wglEnumGpusNV(gpu, &hGPU))
                        break;

                    GPU_DEVICE gpuDevice;
                    gpuDevice.cb = sizeof(gpuDevice);
                    const bool found = wglEnumGpuDevicesNV(hGPU, 0, &gpuDevice);
                    assert(found);

                    std::cout << "GPU " << gpu << ": " << gpuDevice.DeviceString;

                    if (gpuDevice.Flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
                    {
                        const RECT& rect = gpuDevice.rcVirtualScreen;
                        std::cout << " used on [" << rect.left << ' ' << rect.top << ' '
                            << rect.right - rect.left << ' '
                            << rect.bottom - rect.top << ']';
                    }
                    else
                        std::cout << " offline";

                    std::cout << std::endl;
                }
            }
            else
                cout << " Cannot select GPU";
        }
#endif
        // get version info
        cout << endl;

        cout << "  OpenGL vendor  : " << glGetString(GL_VENDOR) << endl;
        cout << "  OpenGL renderer: " << glGetString(GL_RENDERER) << endl;
        cout << "  OpenGL version : " << glGetString(GL_VERSION) << endl;
        cout << "  GLSL   version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

        cout << endl;
        int numBinaryShaderFormats;
        glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &numBinaryShaderFormats);
        if (numBinaryShaderFormats)
        {
            vector<int> binaryFormats(numBinaryShaderFormats);
            glGetIntegerv(GL_SHADER_BINARY_FORMATS, binaryFormats.data());
            cout << " OpenGL Binary formats:" << endl;
            for (int i = 0; i < numBinaryShaderFormats; ++i)
                cout << "   " << ((binaryFormats[i] == GL_SHADER_BINARY_FORMAT_SPIR_V_ARB) ? " SPIR" : "Unknown" + to_string(binaryFormats[i])) << endl;
        }
        cout << "  OpenGL max tessellation level: " << glsGetUInt(GL_MAX_TESS_GEN_LEVEL) << endl;
        cout << "  OpenGL max patch vertices: " << glsGetUInt(GL_MAX_PATCH_VERTICES) << endl;
        cout << "  OpenGL max geometry output vertices: " << glsGetUInt(GL_MAX_GEOMETRY_OUTPUT_VERTICES) << endl;
        cout << "  OpenGL max geometry output components: " << glsGetUInt(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS) << endl;
        cout << endl;
        cout << "  OpenGL max shader storage block size     : " << glsGetUInt(GL_MAX_SHADER_STORAGE_BLOCK_SIZE) << endl;
        cout << "  OpenGL max combined shader storage blocks: " << glsGetUInt(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS) << endl;

        cout << endl;

        ////////////////////////////////////
        //General scene setup
        glEnable(GL_DEBUG_OUTPUT);

        glEnable(GL_DEPTH_TEST); // enable z buffer


    };

public:
    OpenGLMesh* makeBackendMesh(const Mesh& mesh) override
    {
        return new OpenGLMesh(mesh);
    }

    OpenGLPatch* makeBackendPatch(const Patch& patch) override
    {
        return new OpenGLPatch(patch);
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
    static vector<pair<const GLFWwindow*, OpenGLWindowImpl*>> s_allWindows;
    static OpenGLWindowImpl* findWindow(const GLFWwindow* wtf)
    {
        for (const auto& p : s_allWindows)
            if (p.first == wtf)
                return p.second;
        return nullptr;
    }
};

vector<pair<const GLFWwindow*, OpenGLWindowImpl*>> OpenGLWindowImpl::s_allWindows = {};



void OpenGLWindowImpl::glfwScrollCallback(GLFWwindow* glfwWindow, double dxoffsetD, double yoffsetD)
{
    if (const auto windowPtr = findWindow(glfwWindow))
        windowPtr->mouseWheelCallback(Vector2(toFloat(dxoffsetD), toFloat(yoffsetD)));
}
void OpenGLWindowImpl::glfwMousePosCallback(GLFWwindow* glfwWindow, double xpos, double ypos)
{
    const Vector2 newPos(toFloat(xpos), toFloat(ypos));
    if (const auto windowPtr = findWindow(glfwWindow))
        windowPtr->mouseMoveCallback(newPos);
}
void OpenGLWindowImpl::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

OpenGLWindow* OpenGLWindow::make(OpenGLContext& context)
{
    return new OpenGLWindowImpl(context);
}
