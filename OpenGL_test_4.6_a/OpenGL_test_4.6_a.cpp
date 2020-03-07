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

void glfwErrorCallback(int error, const char* description)
{
    cerr << " GLFW error: " << description << endl;
}
void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void glfwMousePosCallback(GLFWwindow* window, double xpos, double ypos);

void glfwWindowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width, height);
}

void testOpenGL0(GLFWwindow* const window, const Mesh& mesh);

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
    const auto meshPtr = MeshLoader::loadMesh(fileName);
    if (meshPtr)
    {
        if (glfwInit())
        {
            glfwSetErrorCallback(glfwErrorCallback);
            //glfwWindowHint(GLFW_SAMPLES, 4);  //FSAA?
            if (GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGLTest", nullptr, nullptr))
            {
                glfwMakeContextCurrent(window);
                glfwMaximizeWindow(window);
                glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
                glfwSetScrollCallback(window, glfwScrollCallback);
                glfwSetCursorPosCallback(window, glfwMousePosCallback);
                glfwSetKeyCallback(window, glfwKeyCallback);
                //                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                if (glfwRawMouseMotionSupported())
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                {
                    int width, height;
                    glfwGetFramebufferSize(window, &width, &height);
                    glViewport(0, 0, width, height);

                    //                glfwSetKeyCallback(window, key_callback);

                    glfwMakeContextCurrent(window);
                    // start GLEW extension handler
                    glewExperimental = GL_TRUE;
                    glewInit();

                    {
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
                                cout << "   " <<  ((binaryFormats[i]== GL_SHADER_BINARY_FORMAT_SPIR_V_ARB) ? " SPIR" : "Unknown"+to_string(binaryFormats[i]) ) << endl;
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

                    testOpenGL0(window, *meshPtr);
                }
                glfwDestroyWindow(window);
            }
            else
            {
                cerr << " GLFW: failed to create window" << endl;
            }
            glfwTerminate();
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

Camera sceneCamera;

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
//float viewerZOffset = 2.0f; //Ugly
//float viewerZAngle = 0.0f;
//Vector2 viewerPanOffset;
//Matrix4x4 patchModelMatrix = makeTranslationMatrix(Vector3(-1.5f, 0.0f, -1.5f));

bool needUpdate = true;
void requestUpdate()
{
    needUpdate = true;
}
bool updateNeeded()
{
    return needUpdate;
}

void updateFinished()
{
    needUpdate = false;
}



void testOpenGL0(GLFWwindow* const window, const Mesh& mesh)
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
    const auto vao = insertMesh(mesh);
    ////////////////////////////////////////////////////////////////
    //Shaders
    ////////////////////////////////////////////////////////////////
    const auto meshShaderProgram = makeShaderProgram("MeshVertexShader.glsl", "MeshFragmentShader.glsl", "MeshGeometryShader.glsl", "mesh");
    const auto edgeShaderProgram = makeShaderProgram("EdgeVertexShader.glsl", "EdgeFragmentShader.glsl", "EdgeGeometryShader.glsl", "edge");
    const auto lineShaderProgram = makeShaderProgram("LineVertexShader.glsl", "LineFragmentShader.glsl", "", "line");

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
    const auto modelCenter = (boundingBox[0] + boundingBox[1]) / 2;
    const auto modelRadius = length(boundingBox[1] - boundingBox[0]) * 0.5f;

    const Vector3 light0Position = modelCenter + Vector3(0.0f, 1.0f,3.0f) * modelRadius * 2;//Vector3(-1.0f, 0.0f,-3.0f)* modelRadius;
    const Vector3 light1Position = modelCenter + Vector3(0.0f, -1.0f,-3.0f) * modelRadius * 2;//Vector3(-1.0f, 0.0f,-3.0f)* modelRadius;

    cout << endl;
    cout << " Model: V=" << mesh.numVertices() << " F=" << mesh.numFaces();
    cout << "  center: " << modelCenter << endl;
    cout << "  radius: " << modelRadius << endl;
    float theta = 0;
    float dist = 0;
    int frameIndex = 0;
    while (!glfwWindowShouldClose(window)) 
    {
        if (!updateNeeded())
        {
            glfwPollEvents();
            continue;
        }
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLErrors();

        const auto viewMatrix = sceneCamera.makeViewMatrix();// (viewerPosition, target, up);
        
        const auto viewportDimensions = glGetViewportDimensions();
        const auto projectionMatrix = sceneCamera.makeProjectionMatrix(viewportDimensions[0]/ viewportDimensions[1]);

        //////////////////////
        //Patch sphere
        constexpr bool renderSphere = false;
        if (renderSphere)
        {
            const auto tcsShader = makeSingleShader(GL_TESS_CONTROL_SHADER,    "SphereTesselationControlShader2.glsl",    "Sphere_TCS");
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

            const Vector3 light0Position = target + Vector3(100.0f,  0.0f, 0.0f);
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
            // Calculate the model matrix for the mesh
            const auto cost = cosf(theta);
            const auto sint = sinf(theta);
            //unitMatrix4x4;
            const auto rotationAroundXBy90Matrix = Matrix4x4{
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f,-1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
            };
            const auto rotationAroundYMatrix = Matrix4x4{
                    cost, 0.0f, sint, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    -sint, 0.0f, cost, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f,
            };
            Matrix4x4 modelMatrix =
                makeTranslationMatrix(modelCenter) *
                rotationAroundYMatrix *
                rotationAroundXBy90Matrix *
                makeTranslationMatrix(-modelCenter)
                ;

            constexpr bool renderMeshFaces = true;
            if (renderMeshFaces)
            {
                glUseProgram(meshShaderProgram);
                checkGLErrors();

                glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "modelMatrix"), 1, true, modelMatrix.data());
                glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
                glUniformMatrix4fv(glGetUniformLocation(meshShaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());
                checkGLErrors();

                glUniform3fv(glGetUniformLocation(meshShaderProgram, "light0Position"), 1, light0Position.data());
                glUniform3fv(glGetUniformLocation(meshShaderProgram, "light0Color"), 1, Vector3(1.0f, 1.0f, 1.0f).data());
                glUniform1f(glGetUniformLocation(meshShaderProgram, "light0SpecularExponent"), 10.0f);
                glUniform3fv(glGetUniformLocation(meshShaderProgram, "light1Position"), 1, light1Position.data());
                glUniform3fv(glGetUniformLocation(meshShaderProgram, "light1Color"), 1, Vector3(1.0f, 1.0f, 1.0f).data());
                glUniform1f(glGetUniformLocation(meshShaderProgram, "light1SpecularExponent"), 100.0f);
                checkGLErrors();

                glUniform3fv(glGetUniformLocation(meshShaderProgram, "viewerPosition"), 1, sceneCamera.m_position.data());
                checkGLErrors();

                glBindVertexArray(vao);
                checkGLErrors();

                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT, GL_FILL);
                glEnable(GL_CULL_FACE);
                const auto numFaces = mesh.numFaces();
                glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, 0);
                checkGLErrors();
            }

            constexpr bool renderWireframe = false;
            if (renderWireframe)
            {
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
                const auto numFaces = mesh.numFaces();
                glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, 0);
                checkGLErrors();
            }
            ///////////////////////
            //Bounding box
            constexpr bool showBoundingBox = false;
            if (showBoundingBox)
            {
                glUseProgram(lineShaderProgram);
                glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "modelMatrix"), 1, true, modelMatrix.data());
                glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
                glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());
                checkGLErrors();

                const auto bb = boundingBox;
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

        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);

        checkGLErrors();
 
        frameIndex++;
        updateFinished();

        // update other events like input handling 
        glfwPollEvents();
        if (!updateNeeded())
            glfwWaitEvents();

    }

}

float toFloat(const double d) { return static_cast<float>(d); }
void glfwScrollCallback(GLFWwindow* window, double dxoffsetD, double yoffsetD)
{
    const auto dy = toFloat(yoffsetD);
    constexpr float factor = 1.0f;
    const auto forward = sceneCamera.m_target - sceneCamera.m_position;
    const auto distance = length(forward);
    constexpr float minimalDistance = 2.0f;
    const auto newDistance = max(distance-dy*factor,minimalDistance);
    sceneCamera.m_position = sceneCamera.m_target - normalize(forward) * newDistance;
    requestUpdate();
}

bool oldPosValid = false;
Vector2 oldPos;
void glfwMousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    const Vector2 newPos(toFloat(xpos), toFloat(ypos));
    if (oldPosValid)
    {
        const auto delta = newPos - oldPos;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            constexpr float factor = 0.01f;
            auto viewMatrix = sceneCamera.makeViewMatrix();
            const auto forwardLength = length(sceneCamera.m_target - sceneCamera.m_position);
            auto viewMatrixInverse = transpose(viewMatrix);
            viewMatrix.at(2,3) -= forwardLength; //Move target to origin
            for (int i = 0; i < 3; ++i)
            {
                viewMatrixInverse.at(i, 3) = sceneCamera.m_target[i];
                viewMatrixInverse.at(3, i) = 0.0f;
            }

            const auto rotationMatrix = makeXRotationMatrix(-factor * delta[1]) * makeYRotationMatrix(factor * delta[0]);
            const auto matrix = viewMatrixInverse * rotationMatrix * viewMatrix;
            sceneCamera.m_position = mulHomogeneous(matrix,sceneCamera.m_position);
            sceneCamera.m_up       = mulHomogeneous(matrix,sceneCamera.m_up);
            requestUpdate();

        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
//            viewerPanOffset += delta;
        }
    }
    oldPos = newPos;
    oldPosValid = true;
}

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

