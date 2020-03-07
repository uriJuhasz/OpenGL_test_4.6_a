//#include "Math.h"
#include "Geometry/Mesh.h"

#include "IO/MeshLoader.h"

#include "Scene/Scene.h"

#include "OpenGLBackend/OpenGLUtilities.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

#include <GL/glew.h>
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

    bool isValid() const { return m_valid; }

private:
    static void glfwErrorCallback(int error, const char* description)
    {
        cerr << " GLFW error: " << description << endl;
    }

private:
    bool m_valid = false;
//    vector<GLFWWindow*> windows
};

class GLFWWindow
{
public:
    explicit GLFWWindow(const GLFWContext& context, const std::string& title) : m_context(context)
    {
        if (!context.isValid())
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

            initOpenGL();
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

    array<int, 2> getFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return { width,height };
    }
    bool isValid() const { return m_window != nullptr; }

public:
    void eventLoop()
    {
        setupScene();

        int frameIndex = 0;

        while (!glfwWindowShouldClose(m_window))
        {
            if (!updateNeeded())
            {
                glfwWaitEvents();
                continue;
            }

            renderScene();

            // put the stuff we've been drawing onto the display
            glfwSwapBuffers(m_window);

            checkGLErrors();

            frameIndex++;
            updateFinished();

            // update other events like input handling 
            glfwPollEvents();
        }
    }
public:
    virtual void initOpenGL()
    {
        //Initialize OpenGL
        {
            const auto fbSize = getFramebufferSize();
            glViewport(0, 0, fbSize[0], fbSize[1]);

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
                    cout << "   " << ((binaryFormats[i] == GL_SHADER_BINARY_FORMAT_SPIR_V_ARB) ? " SPIR" : "Unknown" + to_string(binaryFormats[i])) << endl;
            }
            cout << "  OpenGL max tessellation level: " << glGetUInt(GL_MAX_TESS_GEN_LEVEL) << endl;
            cout << "  OpenGL max patch vertices: " << glGetUInt(GL_MAX_PATCH_VERTICES) << endl;
            cout << "  OpenGL max geometry output vertices: " << glGetUInt(GL_MAX_GEOMETRY_OUTPUT_VERTICES) << endl;
            cout << "  OpenGL max geometry output components: " << glGetUInt(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS) << endl;
            cout << endl;
            cout << "  OpenGL max shader storage block size     : " << glGetUInt(GL_MAX_SHADER_STORAGE_BLOCK_SIZE) << endl;
            cout << "  OpenGL max combined shader storage blocks: " << glGetUInt(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS) << endl;

            cout << endl;
        }
    };
    virtual void setupScene();
    virtual void renderScene();
    virtual void setMesh(unique_ptr<Mesh> m) { m_mesh = move(m); }
    
    unique_ptr<Mesh> m_mesh;
    GLuint m_meshShaderProgram = 0, m_meshEdgeShaderProgram = 0, m_lineShaderProgram = 0;
    GLuint m_meshVAO = 0;

    array<Vector3,2> m_meshBoundingBox;

    Vector3 m_light0Position, m_light1Position;

    Camera m_sceneCamera;

private:
    const GLFWContext& m_context;
    GLFWwindow* m_window = nullptr;
    static const int c_defaultWidth  = 800;
    static const int c_defaultHeight = 800;

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
protected:
    virtual void mouseWheelCallback(const Vector2& wheelDelta)
    {
        auto& sceneCamera = m_sceneCamera;
        const auto dy = wheelDelta[1];
        constexpr float factor = 1.0f;
        const auto forward = sceneCamera.m_target - sceneCamera.m_position;
        const auto distance = length(forward);
        constexpr float minimalDistance = 2.0f;
        const auto newDistance = max(distance - dy * factor, minimalDistance);
        sceneCamera.m_position = sceneCamera.m_target - normalize(forward) * newDistance;
        requestUpdate();
    }

    virtual void mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos)
    {
        if (isMiddleMouseButtonPressed())
        {
            auto& sceneCamera = m_sceneCamera;

            constexpr float factor = 0.01f;
            auto viewMatrix = sceneCamera.makeViewMatrix();
            const auto forwardLength = length(sceneCamera.m_target - sceneCamera.m_position);
            auto viewMatrixInverse = transpose(viewMatrix);
            viewMatrix.at(2, 3) -= forwardLength; //Move target to origin
            for (int i = 0; i < 3; ++i)
            {
                viewMatrixInverse.at(i, 3) = sceneCamera.m_target[i];
                viewMatrixInverse.at(3, i) = 0.0f;
            }

            const auto rotationMatrix = makeXRotationMatrix(-factor * delta[1]) * makeYRotationMatrix(factor * delta[0]);
            const auto matrix = viewMatrixInverse * rotationMatrix * viewMatrix;
            sceneCamera.m_position = mulHomogeneous(matrix, sceneCamera.m_position);
            sceneCamera.m_up = mulHomogeneous(matrix, sceneCamera.m_up);
            requestUpdate();

        }
        else if (isRightMouseButtonPressed())
        {
        }
    }

private:
    Vector2 m_oldMousePos;
    bool m_oldMousePosValid = false;
    void mouseMoveCallback(const Vector2& newPos)
    {
        if (m_oldMousePosValid)
        {
            const auto delta = newPos - m_oldMousePos;
            mouseMoveCallback(delta, m_oldMousePos, newPos);
        }
        m_oldMousePos = newPos;
        m_oldMousePosValid = true;
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
    void requestUpdate()
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
        const GLFWContext context;
        if (context.isValid())
        {
            GLFWWindow window(context, "OpenGLTest");
            if (window.isValid())
            {
                window.setMesh(move(meshPtr));
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

string loadShader(const string& fileName)
{
    ifstream f(fileName);
    string r;
    if (f)
    {
        stringstream buffer;
        buffer << f.rdbuf();
        r = buffer.str();
    }

    return r;
}

template<unsigned int D>ostream& operator<<(ostream& s, const Vector<D>& v)
{
    s << "[";
    for (int i = 0; i < D; ++i)
    {
        s << (i == 0 ? "" : " ") << v[i];
    }
    s << "]";
    return s;
}

static const string shaderBasePath = R"(C:\Users\rossd\source\repos\OpenGL_test_4.6_a\OpenGL_test_4.6_a\shaders\)";
GLuint makeSingleShader(const GLenum  shaderType, const string& shaderPath, const string& title)
{
    const auto shader = glCreateShader(shaderType);
    const auto shaderSource = loadShader(shaderBasePath + shaderPath);
    const auto shaderSourcePtr = shaderSource.c_str();
    glShaderSource(shader, 1, &shaderSourcePtr, nullptr);
    glCompileShader(shader);
    checkShaderErrors(title, shader);

    checkGLErrors();
    return shader;
}
GLuint makeShaderProgram(const string& vertexShaderFilename, const string& fragmentShaderFilename, const string& geometryShaderFilename, const string& title)
{
    const auto shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, makeSingleShader(GL_VERTEX_SHADER, vertexShaderFilename, title + "_Vertex"));
    glAttachShader(shaderProgram, makeSingleShader(GL_FRAGMENT_SHADER, fragmentShaderFilename, title + "_Fragment"));

    if (!geometryShaderFilename.empty())
    {
        glAttachShader(shaderProgram, makeSingleShader(GL_GEOMETRY_SHADER, geometryShaderFilename, title + "_Geometry"));
    }


    glLinkProgram(shaderProgram);
    checkShaderProgramErrors(title, shaderProgram);

    checkGLErrors();

    return shaderProgram;
}

GLuint makeSingleShaderCC(const GLenum  shaderType, const string& shaderSource)
{
    const auto shader = glCreateShader(shaderType);
    const auto ccString = string(
        (shaderType == GL_VERTEX_SHADER) ? "VS" :
        (shaderType == GL_TESS_CONTROL_SHADER) ? "TCS" :
        (shaderType == GL_TESS_EVALUATION_SHADER) ? "TES" :
        (shaderType == GL_GEOMETRY_SHADER) ? "GS" :
        (shaderType == GL_FRAGMENT_SHADER) ? "FS" :
        "");
    const auto defineString = "#version 430\n#define COMPILING_" + ccString + "\n";
    array<const char*, 2> ptrs = {
        defineString.c_str(),
        shaderSource.c_str()
    };
    glShaderSource(shader, 2, ptrs.data(), nullptr);
    glCompileShader(shader);
    checkShaderErrors(ccString, shader);

    checkGLErrors();
    return shader;
}

GLuint makeTessellationShaderProgram(const string& fileName, const string& title)
{
    const auto shaderSource = loadShader(shaderBasePath + fileName);
    const auto shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, makeSingleShaderCC(GL_VERTEX_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_TESS_CONTROL_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_TESS_EVALUATION_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_GEOMETRY_SHADER, shaderSource));
    glAttachShader(shaderProgram, makeSingleShaderCC(GL_FRAGMENT_SHADER, shaderSource));
    glLinkProgram(shaderProgram);
    checkShaderProgramErrors(title, shaderProgram);

    checkGLErrors();

    return shaderProgram;
}

void GLFWWindow::setupScene()
{
    ////////////////////////////////////
    //General scene setup
    glEnable(GL_DEBUG_OUTPUT);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing

    /////////////////////////////////////////
    /////////////////////////////////////////
    //Mesh setup
    //Vertex buffers
    const auto& mesh = *m_mesh;
    const auto vao = insertMesh(mesh);
    m_meshVAO = vao;
    ////////////////////////////////////////////////////////////////
    //Shaders
    ////////////////////////////////////////////////////////////////
    const auto meshShaderProgram = makeShaderProgram("MeshVertexShader.glsl", "MeshFragmentShader.glsl", "MeshGeometryShader.glsl", "mesh");
    const auto edgeShaderProgram = makeShaderProgram("EdgeVertexShader.glsl", "EdgeFragmentShader.glsl", "EdgeGeometryShader.glsl", "edge");
    const auto lineShaderProgram = makeShaderProgram("LineVertexShader.glsl", "LineFragmentShader.glsl", "", "line");
    m_meshShaderProgram = meshShaderProgram;
    m_meshEdgeShaderProgram = edgeShaderProgram;
    m_lineShaderProgram = lineShaderProgram;

    const auto& vertices = mesh.m_vertices;
    const auto numVertices = mesh.numVertices();
    array<Vector3, 2> boundingBox{ vertices[0],vertices[0] };
    {
        const auto& vertices = mesh.m_vertices;
        for (int vi = 1; vi < numVertices; ++vi)
        {
            const auto& vertex = vertices[vi];
            for (int i = 0; i < 3; ++i)
            {
                boundingBox[0][i] = min(boundingBox[0][i], vertex[i]);
                boundingBox[1][i] = max(boundingBox[1][i], vertex[i]);
            }
        }
    }
    m_meshBoundingBox = boundingBox;
    const auto modelCenter = (boundingBox[0] + boundingBox[1]) / 2;
    const auto modelRadius = length(boundingBox[1] - boundingBox[0]) * 0.5f;

    m_light0Position = modelCenter + Vector3(0.0f, 1.0f, 3.0f) * modelRadius * 2;//Vector3(-1.0f, 0.0f,-3.0f)* modelRadius;
    m_light1Position = modelCenter + Vector3(0.0f, -1.0f, -3.0f) * modelRadius * 2;//Vector3(-1.0f, 0.0f,-3.0f)* modelRadius;

    cout << endl;
    cout << " Model: V=" << mesh.numVertices() << " F=" << mesh.numFaces();
    cout << "  center: " << modelCenter << endl;
    cout << "  radius: " << modelRadius << endl;

}
void GLFWWindow::renderScene()
{
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkGLErrors();

    const auto sceneCamera = m_sceneCamera;
    const auto viewMatrix = sceneCamera.makeViewMatrix();// (viewerPosition, target, up);

    const auto viewportDimensions = glGetViewportDimensions();
    const auto projectionMatrix = sceneCamera.makeProjectionMatrix(viewportDimensions[0] / viewportDimensions[1]);

    //////////////////////
    //Patch sphere
    constexpr bool renderSphere = false;
    if (renderSphere)
    {
        const auto tcsShader = makeSingleShader(GL_TESS_CONTROL_SHADER, "SphereTesselationControlShader2.glsl", "Sphere_TCS");
        const auto tesShader = makeSingleShader(GL_TESS_EVALUATION_SHADER, "SphereTesselationEvaluationShader2.glsl", "Sphere_TES");
        const auto vertexShader = makeSingleShader(GL_VERTEX_SHADER, "SphereTesselationVertexShader.glsl", "Sphere_Vertex");
        const auto fragmentShader = makeSingleShader(GL_FRAGMENT_SHADER, "SphereTesselationFragmentShader.glsl", "Sphere_Fragment");

        const auto shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, tcsShader);
        glAttachShader(shaderProgram, tesShader);
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        checkGLErrors();

        glUseProgram(shaderProgram);
        checkGLErrors();

        glUniform1f(glGetUniformLocation(shaderProgram, "uDetail"), 10.0f);
        //            glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, true, unitMatrix4x4.data());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());

        checkGLErrors();

        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPatchParameteri(GL_PATCH_VERTICES, 1); // #of vertices in each patch
        glBegin(GL_PATCHES);
        glVertex4f(0.0f, 0.0f, 0.0f, 10.0f);
        glVertex4f(0.0f, 0.0f, -50.0f, 50.0f);
        glEnd();
    }

    //////////////////////
    //Bezier patch
    constexpr bool showBezierPatch = true;
    if (showBezierPatch)
    {
        const auto shaderProgram = makeTessellationShaderProgram("BezierShaderProgram.glsl", "Bezier");
        checkGLErrors();
        glUseProgram(shaderProgram);
        checkGLErrors();

        const auto numPatchVertices = 16;
        glPatchParameteri(GL_PATCH_VERTICES, numPatchVertices);
        GLuint vao = 0;
        {
            const array<Vector3, 16> patchParameters = {
                Vector3(0,2,0), Vector3(1, 1,0), Vector3(2, 1,0), Vector3(3, 2,0),
                Vector3(0,1,1), Vector3(1,-2,1), Vector3(2, 1,1), Vector3(3, 0,1),
                Vector3(0,0,2), Vector3(1, 1,2), Vector3(2, 0,2), Vector3(3,-1,2),
                Vector3(0,0,3), Vector3(1, 1,3), Vector3(2,-1,3), Vector3(3,-1,3)
            };
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            //Vertex positions
            GLuint vertexBuffer = 0;
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, patchParameters.size() * sizeof(patchParameters[0]), patchParameters.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            checkGLErrors();
        }

        const Matrix4x4 modelMatrix = makeTranslationMatrix({ -1.5f,0.0f,-1.5f });
        const Vector3 target(0.0f, 0.0f, 0.0f);

        const auto viewMatrix = sceneCamera.makeViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, true, modelMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());

        const Vector3 light0Position = target + Vector3(100.0f, 0.0f, 0.0f);
        const Vector3 light1Position = target + Vector3(0.0f, -1000.0f, 0.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "light0Position"), 1, light0Position.data());
        glUniform3fv(glGetUniformLocation(shaderProgram, "light0Color"), 1, Vector3(1.0f, 1.0f, 1.0f).data());
        glUniform1f(glGetUniformLocation(shaderProgram, "light0SpecularExponent"), 1000.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "light1Position"), 1, light1Position.data());
        glUniform3fv(glGetUniformLocation(shaderProgram, "light1Color"), 1, Vector3(1.0f, 1.0f, 1.0f).data());
        glUniform1f(glGetUniformLocation(shaderProgram, "light1SpecularExponent"), 100.0f);
        checkGLErrors();

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewerPosition"), 1, sceneCamera.m_position.data());
        checkGLErrors();

        const Vector3 frontColor(1.0f, 0.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "frontColor"), 1, frontColor.data());
        const Vector3 backColor(0.1f, 0.3f, 0.3f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "backColor"), 1, backColor.data());
        checkGLErrors();

        glUniform1f(glGetUniformLocation(shaderProgram, "tessellationLevel"), 64.0f);
        checkGLErrors();


        glDisable(GL_CULL_FACE);
        constexpr bool showPatchFaces = true;
        if (showPatchFaces)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// LINE);
            glUniform1i(glGetUniformLocation(shaderProgram, "edgeMode"), 0);
            glDepthFunc(GL_LESS);

            glBindVertexArray(vao);
            glDrawArrays(GL_PATCHES, 0, 16);
            checkGLErrors();
        }
        constexpr bool showPatchEdges = true;
        if (showPatchEdges)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUniform1i(glGetUniformLocation(shaderProgram, "edgeMode"), 1);
            glLineWidth(1.0f);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0, 0.0);
            glBindVertexArray(vao);
            glDrawArrays(GL_PATCHES, 0, 16);
            checkGLErrors();
        }
    }
    //////////////////////
    //Render mesh
    constexpr bool renderMesh = false;
    if (renderMesh)
    {
        const auto modelMatrix = unitMatrix4x4;

        const auto& mesh = *m_mesh;
        const auto numFaces = mesh.numFaces();

        constexpr bool renderMeshFaces = true;
        if (renderMeshFaces)
        {
            const auto meshShaderProgram = m_meshShaderProgram;
            glUseProgram(meshShaderProgram);
            checkGLErrors();

            glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "modelMatrix"), 1, true, modelMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());
            checkGLErrors();

            glUniform3fv(glGetUniformLocation(meshShaderProgram, "light0Position"), 1, m_light0Position.data());
            glUniform3fv(glGetUniformLocation(meshShaderProgram, "light0Color"), 1, Vector3(1.0f, 1.0f, 1.0f).data());
            glUniform1f(glGetUniformLocation(meshShaderProgram, "light0SpecularExponent"), 10.0f);
            glUniform3fv(glGetUniformLocation(meshShaderProgram, "light1Position"), 1, m_light1Position.data());
            glUniform3fv(glGetUniformLocation(meshShaderProgram, "light1Color"), 1, Vector3(1.0f, 1.0f, 1.0f).data());
            glUniform1f(glGetUniformLocation(meshShaderProgram, "light1SpecularExponent"), 100.0f);
            checkGLErrors();

            glUniform3fv(glGetUniformLocation(meshShaderProgram, "viewerPosition"), 1, sceneCamera.m_position.data());
            checkGLErrors();

            glBindVertexArray(m_meshVAO);
            checkGLErrors();

            glDepthFunc(GL_LESS);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_CULL_FACE);
            glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, 0);
            checkGLErrors();
        }

        constexpr bool renderWireframe = false;
        if (renderWireframe)
        {
            const auto edgeShaderProgram = m_meshEdgeShaderProgram;
            glUseProgram(edgeShaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(edgeShaderProgram, "modelMatrix"), 1, true, modelMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(edgeShaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(edgeShaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());

            glLineWidth(1.0f);
            //        glEnable(GL_BLEND);
            //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //        glEnable(GL_LINE_SMOOTH);
            //        glEnable(GL_POLYGON_SMOOTH);
            //        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            //        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
            //        glDisable(GL_CULL_FACE);
            const Vector3 edgeColor(1.0f, 1.0f, 1.0f);
            glUniform3fv(glGetUniformLocation(edgeShaderProgram, "edgeColor"), 1, edgeColor.data());
            checkGLErrors();

            glPolygonMode(GL_FRONT, GL_LINE);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0, 0.0);
            glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, 0);
            checkGLErrors();
        }
        ///////////////////////
        //Bounding box
        constexpr bool showBoundingBox = false;
        if (showBoundingBox)
        {
            const auto lineShaderProgram = m_lineShaderProgram;
            glUseProgram(lineShaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "modelMatrix"), 1, true, modelMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());
            checkGLErrors();

            const auto bb = m_meshBoundingBox;
            const array<Vector3, 8> boundingBoxVertices = {
                Vector3(bb[0][0],bb[0][1],bb[0][2]),
                Vector3(bb[1][0],bb[0][1],bb[0][2]),
                Vector3(bb[1][0],bb[1][1],bb[0][2]),
                Vector3(bb[0][0],bb[1][1],bb[0][2]),
                Vector3(bb[0][0],bb[0][1],bb[1][2]),
                Vector3(bb[1][0],bb[0][1],bb[1][2]),
                Vector3(bb[1][0],bb[1][1],bb[1][2]),
                Vector3(bb[0][0],bb[1][1],bb[1][2])
            };

            const array<int, 24> bbEdges = {
                0,1, 1,2, 2,3, 3,0,
                4,5, 5,6, 6,7, 7,4,
                0,4, 1,5, 2,6, 3,7
            };

            for (int e = 0; e < 12; ++e)
            {
                const auto v0 = boundingBoxVertices[bbEdges[2 * e + 0]];
                const auto v1 = boundingBoxVertices[bbEdges[2 * e + 1]];
                glLineWidth(2.0f);
                glColor3f(1.0f, 0.0f, 0.0f);
                glBegin(GL_LINES);
                glVertex3fv(v0.data());
                glVertex3fv(v1.data());
                //                    glVertex3f(v1[0], v1[1], v1[2]);
                glEnd();
            }
        }
    }
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

