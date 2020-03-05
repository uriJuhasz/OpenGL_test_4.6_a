#include "Math.h"
#include "Mesh.h"

#include "MeshLoader.h"

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
//    const auto fileName = R"(C:\Users\rossd\Downloads\90-3ds\3ds\Dragon 2.5_3ds.3ds)";
    const auto fileName = R"(C:\Users\rossd\Downloads\Cat_v1_L2.123c6a1c5523-ac23-407e-9fbb-d0649ffb5bcb\12161_Cat_v1_L2.obj)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\Scorpio N05808.3ds)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\fc6bdb2aea4b58c23a3e8d4e87fba763\Elephant N090813.3DS)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\a8cfcfd0082c61bad7aa4fbd1c57a277\Ship hms victory frigate nelson N270214.3DS)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\e6eadc4ff882b84784dd133168c1f099\Autogenerator BelMag N170211.3DS)";
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
                        {
                            GLint maxPatchVertices = 0;
                            glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
                            cout << "  OpenGL max patch vertices: " << maxPatchVertices << endl;
                        }
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
string toString(const vector<char>& v)
{
    string r; r.reserve(v.size());
    for (const auto c : v)
        r.push_back(c);
    return r;
}

void checkShaderErrors(const string& shaderType, const GLuint s)
{
    int infoLength;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0)
    {
        vector<char> info(infoLength, ' ');
        glGetShaderInfoLog(s, infoLength, &infoLength, info.data());
        const auto infoString = toString(info);
        cerr << shaderType << " shader log: " << endl << infoString;
    }
}
void checkShaderProgramErrors(const string& shaderType, const GLuint p)
{
    int infoLength;
    glGetProgramiv(p, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0)
    {
        vector<char> info(infoLength, ' ');
        glGetProgramInfoLog(p, infoLength, &infoLength, info.data());
        const auto infoString = toString(info);
        cerr << shaderType << " shader program log: " << endl << infoString << endl;
    }
}

void checkGLErrors()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        cerr << " OpenGL error " << err << " : " << gluErrorString(err) << endl;
        cerr << "";
    }
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
    const auto defineString = R"(#version 410\n #define COMPILING_)" + ccString + "\n";
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

float viewerZOffset = 0.0f; //Ugly
float viewerZAngle = 0.0f;
Vector2 viewerPanOffset;


GLuint insertMesh(const Mesh& mesh)
{
    const int numVertices = mesh.numVertices();
    const auto& vertices = mesh.m_vertices;

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Vertex positions
    GLuint vertexBuffer = 0;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    checkGLErrors();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    checkGLErrors();

    //Vertex normals
    const auto& normals = mesh.m_normals;
    assert(normals.size() == numVertices);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
    checkGLErrors();
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
    checkGLErrors();

    //UV coords
    const auto& uvCoords = mesh.m_textureCoords;
    assert(uvCoords.size() == numVertices);

    GLuint uvcoordBuffer;
    glGenBuffers(1, &uvcoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvcoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(uvCoords[0]), uvCoords.data(), GL_STATIC_DRAW);
    checkGLErrors();

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvcoordBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, nullptr);
    checkGLErrors();

    //Faces
    const auto& faces = mesh.m_faces;
    const int numFaces = mesh.numFaces();
    GLuint fao;
    glGenBuffers(1, &fao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fao);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * sizeof(faces[0]), faces.data(), GL_STATIC_DRAW);

    checkGLErrors();
    return vao;
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

    /////////////
 /*   {
        vector<int> vertexNumFaces(numVertices, 0);
        for (const auto& face : faces)
            for (int i = 0; i < 3; ++i)
                vertexNumFaces[face.m_vis[i]]++;

        for (int vi = 0; vi < numVertices; ++vi)
            if (vertexNumFaces[vi] == 0)
                cout << "   Vertex " << vi << " is an orphan";
    }
    */
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
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(meshShaderProgram);
        checkGLErrors();

        // Set the projection matrix in the vertex shader.
        const auto cost = cosf(theta);
        const auto sint = sinf(theta);
            //unitMatrix4x4;
        const auto rotationAroundXBy90Matrix = Matrix4x4{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f,-1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
        const auto rotationAroundYMatrix = Matrix4x4 {
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

        //Setup the view matrix
//        const auto t2 = theta * 2;
        const auto cosx = cosf(viewerZAngle);
        const auto sinx = sinf(viewerZAngle);
//        const auto vd = 3.0f;
        const Vector3 viewerPosition = 
            modelCenter
            + (Vector3(cosx,0,sinx) * (modelRadius * 2 + viewerZOffset)) 
//            + Vector3(viewerPanOffset[0], viewerPanOffset[1],0.0f)
            ;
        const Vector3 up(0.0f, 1.0f, 0.0f);
        const Vector3 target = modelCenter;// (0.0f, 0.0f, 0.0f);
        const auto forward = normalize(target - viewerPosition);
        const auto right = cross(forward, up);
        if (frameIndex == 0)
        {
            cout << "  Viewer:" << viewerPosition << endl;
            cout << "  target:" << target << endl;
            cout << "  forward:" << forward << endl;
            cout << "  up     :" << up << endl;
            cout << "  right  :" << right << endl;
        }
        Matrix4x4 viewMatrix =
        {
                right  [0], right  [1], right  [2], -dot(right,viewerPosition),
                up     [0], up     [1], up     [2], -dot(up,viewerPosition),
                forward[0], forward[1], forward[2], -dot(forward,viewerPosition),
                0.0f,0.0f, 0.0f, 1.0f
        };
                
        array<int, 4> viewport;
        glGetIntegerv(GL_VIEWPORT,viewport.data());
        const float cw = 1;// viewport[2];
        const float ch = 1;// viewport[3];

        const float vpw = static_cast<float>(viewport[2]);
        const float vph = static_cast<float>(viewport[3]);
        const float ar = vpw / vph;
        const auto horizontal = (vpw >= vph);
        const float w = horizontal ? ch * ar : cw;
        const float h = horizontal ? ch      : cw / ar;

        const float n = 1.0f;
        const float f = 100000.0f;
        const float d = f - n;
        Matrix4x4 projectionMatrix =
        {
            2*n/w, 0.0f, 0.0f, 0.0f,
            0.0f, 2*n/h, 0.0f, 0.0f,
            0.0f, 0.0f, (f+n)/d, -2*f*n/d,
            0.0f, 0.0f, 1.0f, 0.0f
        };

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

        glUniform3fv(glGetUniformLocation(meshShaderProgram, "viewerPosition"), 1, viewerPosition.data());
        checkGLErrors();


        //////////////////////
        //Patch sphere
        constexpr bool renderSphere = true;
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
//                glVertex4f(0.0f, 0.0f, 0.0f, 100.0f);
                glVertex4f(0.0f, 0.0f, 0.0f, 10.0f);
                glVertex4f(0.0f, 0.0f, -50.0f, 50.0f);
//                glVertex4f(0.0f, 0.0f, 50.0f, 1000.0f);
            glEnd();
        }

        //////////////////////
        //Bezier patch
        {
            const auto shaderProgram = makeTessellationShaderProgram("BezierShaderProgram.glsl", "Bezier");
            checkGLErrors();
            glUseProgram(shaderProgram);
            checkGLErrors();

            const array<Vector3, 16> patchParameters = {
                Vector3(0,2,0), Vector3(1, 1,0), Vector3(2, 1,0), Vector3(3, 2,0),
                Vector3(0,1,1), Vector3(1,-2,1), Vector3(2, 1,1), Vector3(3, 0,1),
                Vector3(0,0,2), Vector3(1, 1,2), Vector3(2, 0,2), Vector3(3,-1,2),
                Vector3(0,0,3), Vector3(1, 1,3), Vector3(2,-1,3), Vector3(3,-1,3)
            };
            glPatchParameteri(GL_PATCH_VERTICES, 16);
            checkGLErrors();

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, true, unitMatrix4x4.data());
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());

            glDisable(GL_CULL_FACE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_PATCHES);
            for (int i = 0; i < patchParameters.size(); ++i)
                glVertex3fv(patchParameters[i].data());
            glEnd();
            checkGLErrors();
        }
        //////////////////////
        //Render mesh
        constexpr bool renderMesh = true;
        if (renderMesh)
        {
            glUseProgram(meshShaderProgram);
            checkGLErrors();
            glBindVertexArray(vao);
            checkGLErrors();

            glDepthFunc(GL_LESS);
            glPolygonMode(GL_FRONT, GL_FILL);
            glEnable(GL_CULL_FACE);
            const auto numFaces = mesh.numFaces();
            glDrawElements(GL_TRIANGLES, numFaces*3, GL_UNSIGNED_INT, 0);
            checkGLErrors();
        }

        constexpr bool renderWireframe = true;
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
        glBindVertexArray(0);

        ///////////////////////
        //Bounding box
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
                    glVertex3f(v0[0],v0[1],v0[2]);
                    glVertex3f(v1[0], v1[1], v1[2]);
                glEnd();
            }
        }



        // update other events like input handling 
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
        {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
            {
                do {
                    cerr << " OpenGL error: " << err << gluErrorString(err) << endl;
                    err = glGetError();
                } while (err!= GL_NO_ERROR);
            }
        }
        
        
//        theta += 0.01f;
        frameIndex++;
    }

}

float toFloat(const double d) { return static_cast<float>(d); }
void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    constexpr float factor = 50.0f;
    viewerZOffset -= toFloat(yoffset);
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
            constexpr float factor = 0.02f;
            viewerZAngle += factor * delta[0];
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            viewerPanOffset += delta;
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

