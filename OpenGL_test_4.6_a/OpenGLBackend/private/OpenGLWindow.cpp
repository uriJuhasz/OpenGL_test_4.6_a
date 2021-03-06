#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpenGLWindow.h"
#include "OpenGLContext.h"

#include "OpenGLUtilities.h"

#include "Primitives/OpenGLMeshPrimitive.h"
#include "Instances/OpenGLMeshInstance.h"

#include "Primitives/OpenGLBezierPatchPrimitive.h"
#include "Instances/OpenGLBezierPatchInstance.h"

#include "Utilities/Exception.h"
#include "Utilities/Misc.h"

#include "Backend/Scene/PointLight.h"

#include <cassert>
#include <vector>
#include <chrono>

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
//        constexpr int numSamples = 4;
//        glfwWindowHint(GLFW_SAMPLES, numSamples);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        if (const auto window = glfwCreateWindow(c_defaultWidth, c_defaultHeight, title.c_str(), nullptr, nullptr))
        {
            m_glfwWindow = window;
            s_allWindows.push_back({ m_glfwWindow,this });

            glfwMakeContextCurrent(window);
            const auto glewInitResult = glewInit();
            const auto glewOk = (glewInitResult == GLEW_OK);
            if (glewOk)
            {
                cout << "  Using GLEW " << glewGetString(GLEW_VERSION) << endl;

                glfwMaximizeWindow(window);
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glfwSwapBuffers(m_glfwWindow);

                glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
                glfwSetScrollCallback(window, glfwScrollCallback);
                glfwSetCursorPosCallback(window, glfwMousePosCallback);
                glfwSetKeyCallback(window, glfwKeyCallback);

                if (glfwRawMouseMotionSupported())
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

//                glEnable(GL_MULTISAMPLE);
            }
            else
            {

                const auto errorMessage = string("GLEW init failed : ") + string(reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
                cerr << "!Error: " << errorMessage << endl;
                throw new Exception(errorMessage);
            }

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
        if (m_glfwWindow)
        {
            for (int i = 0; i < s_allWindows.size(); ++i) //Remove from list
            {
                if (s_allWindows[i].first == m_glfwWindow)
                {
                    s_allWindows[i] = s_allWindows.back();
                    s_allWindows.resize(s_allWindows.size() - 1);
                }
            }
            glfwDestroyWindow(m_glfwWindow);
        }
    }

    OpenGLContext& getContext() override { return m_context; }

public:
    array<int, 2> getFramebufferSize() const override
    {
        int width, height;
        glfwGetFramebufferSize(m_glfwWindow, &width, &height);
        return { width,height };
    }
    Vector2 getViewportDimensions() const override
    {
        array<int, 4> viewport;
        glGetIntegerv(GL_VIEWPORT, viewport.data());
        const float vpw = static_cast<float>(viewport[2] - viewport[0]);
        const float vph = static_cast<float>(viewport[3] - viewport[1]);
        return Vector2(vpw, vph);
    }

    operator bool() const  override { return m_glfwWindow != nullptr; }

public:
    void registerView(BackendViewInterface* view) override
    {
        m_view = view;
    }

public:
    void eventLoop() override
    {
        if (!m_view)
            return;
        m_view->setupScene();
        glsCheckErrors();

        int frameIndex = 0;

        while (!glfwWindowShouldClose(m_glfwWindow))
        {
            if (!updateNeeded())
            {
                glfwWaitEvents();
                continue;
            }

            const auto frameStartTime = chrono::high_resolution_clock::now();

            // wipe the drawing surface clear
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glsCheckErrors();

            m_view->renderScene();
            glsCheckErrors();

            glfwSwapBuffers(m_glfwWindow);
            glsCheckErrors();

            const auto frameEndTime = chrono::high_resolution_clock::now();
            const auto frameTime = chrono::duration<double>(frameEndTime - frameStartTime).count();
            const auto fps = 1.0 / frameTime;
            const auto title = "fps: " + to_string(fps) ;
            glfwSetWindowTitle(m_glfwWindow, title.c_str());

            glsCheckErrors();

            frameIndex++;
            updateFinished();

            glfwPollEvents();
        }
    }

private:
    OpenGLContext& m_context;
    GLFWwindow* m_glfwWindow = nullptr;
    static const int c_defaultWidth = 800;
    static const int c_defaultHeight = 800;

    BackendViewInterface* m_view;

public:
    bool isLeftMouseButtonPressed() const override
    {
        return glfwGetMouseButton(m_glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    }
    bool isMiddleMouseButtonPressed() const override
    {
        return glfwGetMouseButton(m_glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    }
    bool isRightMouseButtonPressed() const override
    {
        return glfwGetMouseButton(m_glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    }

public:
    void initialize() override
    {
        if (!m_glfwWindow)
            return;
        int w, h;
        glfwGetWindowSize(m_glfwWindow, &w, &h);
        glViewport(0, 0, w, h);

        // start GLEW extension handler
        glewExperimental = GL_TRUE;
        glewInit();

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
                cout << "   " << ((binaryFormats[i] == GL_SHADER_BINARY_FORMAT_SPIR_V_ARB) ? " SPIR-V" : "Unknown" + to_string(binaryFormats[i])) << endl;
        }
        
        cout << "  OpenGL max vertex attributes: " << glsGetUInt(GL_MAX_VERTEX_ATTRIB_BINDINGS) << endl;
        cout << "  OpenGL max tessellation level: " << glsGetUInt(GL_MAX_TESS_GEN_LEVEL) << endl;
        cout << "  OpenGL max patch vertices: " << glsGetUInt(GL_MAX_PATCH_VERTICES) << endl;
        cout << "  OpenGL max geometry output vertices: " << glsGetUInt(GL_MAX_GEOMETRY_OUTPUT_VERTICES) << endl;
        cout << "  OpenGL max geometry output components: " << glsGetUInt(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS) << endl;
        cout << endl;
        cout << "  OpenGL max shader storage block size     : " << (glsGetUInt(GL_MAX_SHADER_STORAGE_BLOCK_SIZE)/(1024*1024)) << "MB" << endl;
        cout << "  OpenGL max shader uniform block size     : " << (glsGetUInt(GL_MAX_UNIFORM_BLOCK_SIZE) / (1024)) << "KB" << endl;
        cout << "  OpenGL max combined shader storage blocks: " << glsGetUInt(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS) << endl;

        cout << endl;

        ////////////////////////////////////
        //General OpenGL setup
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(openGLMessageCallback, this);


        glEnable(GL_DEPTH_TEST); // enable Z buffer

    };

public:
    std::unique_ptr<Scene> makeScene() override
    {
        return OpenGLScene::makeScene(*this);
    }

private:
    static void openGLMessageCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam);
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


/////////////////////////////////////////////////////////////////////////////

string getOpenGLErrorSourceTypeString(const GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return "unknown";
    }
}
string getOpenGLErrorTypeString(const GLenum errorType)
{
    switch (errorType)
    {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        default:return "Unknown";
    }
}

string getOpenGLErrorSeverityString(const GLenum errorType)
{
    switch (errorType)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
    case GL_DEBUG_SEVERITY_LOW: return "LOW";
    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
    default:return "Unknown";
    }
}

void OpenGLWindowImpl::openGLMessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (id == 131185)
        return; //Ignore warning on using Video memory

    const auto sourceTypeString = getOpenGLErrorSourceTypeString(source);
    const auto errorTypeString = getOpenGLErrorTypeString(type);
    const auto severityString = getOpenGLErrorSeverityString(severity);

    std::cerr << "OpenGL Message - source: " << sourceTypeString << ", type: " << errorTypeString << ", severity: " << severityString << ", id: " << id << ": " << message << '\n';
    if (id!=131218) // severity != GL_DEBUG_SEVERITY_NOTIFICATION && type != GL_DEBUG_TYPE_PERFORMANCE)
    {
        std::cerr << "";
    }
}


//Look at GPUs
#ifdef NAMNAMBULU
#ifdef _WIN32
#include <GL/wglew.h>
#endif
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

