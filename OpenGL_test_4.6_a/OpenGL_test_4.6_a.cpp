#include "Math.h"
#include "Mesh.h"

#include "MeshLoader.h"

#include <iostream>
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

void glfwWindowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width, height);
}

void testOpenGL0(GLFWwindow* const window, const Mesh& mesh);

int main()
{
    cout << "start" << endl;

//    const auto fileName = R"(C:\Users\rossd\Downloads\90-3ds\3ds\Dragon 2.5_3ds.3ds)";
    const auto fileName = R"(C:\Users\rossd\Downloads\Cat_v1_L2.123c6a1c5523-ac23-407e-9fbb-d0649ffb5bcb\12161_Cat_v1_L2.obj)";
    cout << " Loading mesh: " << fileName;
    const auto meshPtr = MeshLoader::loadMesh(fileName);
    if (meshPtr)
    {
        if (glfwInit())
        {
            glfwSetErrorCallback(glfwErrorCallback);
            if (GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGLTest", nullptr, nullptr))
            {
                glfwMakeContextCurrent(window);
                glfwSetWindowSizeCallback(window, glfwWindowResizeCallback);
                {
                    int width, height;
                    glfwGetFramebufferSize(window, &width, &height);
                    glViewport(0, 0, width, height);

                    //                glfwSetKeyCallback(window, key_callback);

                    glfwMakeContextCurrent(window);
                    // start GLEW extension handler
                    glewExperimental = GL_TRUE;
                    glewInit();

                    // get version info
                    cout << "  OpenGL vendor  : " << glGetString(GL_VENDOR) << endl;
                    cout << "  OpenGL renderer: " << glGetString(GL_RENDERER) << endl;
                    cout << "  OpenGL version : " << glGetString(GL_VERSION) << endl;
                    cout << "  glsl   version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

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

/*
typedef array<GLuint, 3> Triangle;
array<Triangle, 2> faces = {
    Triangle{0, 1, 2},
    Triangle{0, 2, 3}
};


const array<Vector3, 4> vertices = {
   Vector3(-1.0f,  -1.0f,  0.0f),
   Vector3(1.0f,  -1.0f,  0.0f),
   Vector3(1.0f,   1.0f,  0.0f),
   Vector3(-1.0f,   1.0f,  0.0f)
};

const array<Vector3, 4> normals = {
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f)
};

const array<Vector2, 4> textureUV = {
   Vector2(0.0f,  0.0f),
   Vector2(1.0f,  0.0f),
   Vector2(1.0f,  1.0f),
   Vector2(0.0f,  1.0f)
};
*/


constexpr const char* meshVertexShaderSource = R"(
#version 400

in vec3 position;
in vec3 normal;
in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec2 fragmentUVCoord;

void main() {
  vec4 pos4 = modelMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix*viewMatrix*pos4;
  fragmentPosition = pos4.xyz;
  fragmentNormal = (modelMatrix*vec4(normal,0)).xyz;
  fragmentUVCoord = uvCoord;
})";

constexpr const char* meshFragmentShaderSource = R"(
#version 400

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec2 fragmentUVCoord;

uniform vec3 lightPosition;
uniform vec3 lightPosition1;
uniform vec3 viewerPosition;

out vec4 frag_color;

void main() {
   vec3 lightDirection = normalize(lightPosition-fragmentPosition);
   vec3 normal = normalize(fragmentNormal);
   float diffuseLight = dot(lightDirection,normal);
   vec3 baseColor = 
/*            vec3(1,0,0)*(clamp(1-fragmentUVCoord.x-fragmentUVCoord.y,0,1)+clamp(fragmentUVCoord.x+fragmentUVCoord.y-1.0f,0,1)) + 
            vec3(0,1,0)*(fragmentUVCoord.y-fragmentUVCoord.x) +
            vec3(0,0,1)*(fragmentUVCoord.x-fragmentUVCoord.y) 
*/
            vec3(1,0,0)*(clamp(1-fragmentUVCoord.x-fragmentUVCoord.y,0,1) + clamp(fragmentUVCoord.x-fragmentUVCoord.y,0,1)) + 
            vec3(0,1,0)*(clamp(fragmentUVCoord.y-fragmentUVCoord.x,0,1) + clamp(fragmentUVCoord.x-fragmentUVCoord.y,0,1)) +
            vec3(0,0,1)*(clamp(fragmentUVCoord.x+fragmentUVCoord.y-1.0f,0,1)) 
    ;
   vec3 diffuseColor = diffuseLight * baseColor;
   
   const vec3 lightSpecularColor = vec3(1,1,1);

   float specularExponent = 10;
   vec3 viewerDirection = normalize(viewerPosition-fragmentPosition);
   vec3 reflectedLight = 2*diffuseLight*normal - lightDirection;
   float specularIntensity = pow(clamp(dot(reflectedLight,viewerDirection),0,1),specularExponent);

    vec3 specularColor = specularIntensity*lightSpecularColor;
//   vec3 specularColor = vec3(0,0,0); //lightSpecularColor * 0.3/length(lightDirection);
   frag_color = vec4(diffuseColor+specularColor,1); //objectColor.w);
})";

constexpr const char* lineVertexShaderSource = R"(
#version 400

in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
  vec4 pos4 = modelMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix*viewMatrix*pos4;
})";

constexpr const char* lineFragmentShaderSource = R"(
#version 400

out vec4 frag_color;

void main() {
   frag_color = vec4(1,0,0,1);
})";

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

void checkGLErrors()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        cerr << " OpenGL error: " << err << gluErrorString(err) << endl;
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
void testOpenGL0(GLFWwindow* const window, const Mesh& mesh)
{
    glEnable(GL_DEBUG_OUTPUT);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    //Vertex buffers
//    const int numVertices = static_cast<int>(vertices.size());
    const int numVertices = mesh.numVertices();
    const auto& vertices = mesh.m_vertices;
    
    GLuint vertexBuffer = 0;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    checkGLErrors();

    const auto& normals = mesh.m_normals;
    assert(normals.size() == numVertices);

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
    checkGLErrors();

    const auto& uvCoords = mesh.m_textureCoords;
    assert(uvCoords.size() == numVertices);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(uvCoords[0]), uvCoords.data(), GL_STATIC_DRAW);
    checkGLErrors();

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    checkGLErrors();

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);
    checkGLErrors();

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, nullptr);
    checkGLErrors();

    const auto& faces = mesh.m_faces;
    const int numFaces = mesh.numFaces();
    GLuint fao;
    glGenBuffers(1, &fao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fao);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces*sizeof(faces[0]), faces.data(), GL_STATIC_DRAW);

    checkGLErrors();

    //Shaders
    const auto meshVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(meshVertexShader, 1, &meshVertexShaderSource, nullptr);
    glCompileShader(meshVertexShader);
    checkShaderErrors("Vertex", meshVertexShader);

    checkGLErrors();

    const auto meshFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(meshFragmentShader, 1, &meshFragmentShaderSource, nullptr);
    glCompileShader(meshFragmentShader);
    checkShaderErrors("Fragment", meshFragmentShader);

    checkGLErrors();

    const auto meshShaderProgram = glCreateProgram();
    glAttachShader(meshShaderProgram, meshFragmentShader);
    glAttachShader(meshShaderProgram, meshVertexShader);
    glLinkProgram(meshShaderProgram);

    checkGLErrors();

    /////////////////////
    //Line shader

    const auto lineVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(lineVertexShader, 1, &lineVertexShaderSource, nullptr);
    glCompileShader(lineVertexShader);
    checkShaderErrors("Vertex", meshVertexShader);

    checkGLErrors();

    const auto lineFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lineFragmentShader, 1, &lineFragmentShaderSource, nullptr);
    glCompileShader(lineFragmentShader);
    checkShaderErrors("LineFragment", lineFragmentShader);

    checkGLErrors();

    const auto lineShaderProgram = glCreateProgram();
    glAttachShader(lineShaderProgram, lineFragmentShader);
    glAttachShader(lineShaderProgram, lineVertexShader);
    glLinkProgram(lineShaderProgram);

    checkGLErrors();

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
    array<Vector3, 2> boundingBox{ vertices[0],vertices[0] };
    {
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

    const Vector3 lightPosition = modelCenter + Vector3(0.0f, 1.0f,3.0f) * modelRadius * 2;//Vector3(-1.0f, 0.0f,-3.0f)* modelRadius;


    cout << endl;
    cout << " Model: V=" << numVertices << " F=" << numFaces;
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
//        const auto cosx = cosf(t2);
//        const auto sinx = sinf(t2);
//        const auto vd = 3.0f;
        const Vector3 viewerPosition = modelCenter + Vector3(0,0,1) * modelRadius * 2;// (vd * cosx, 0, vd * sinx);
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
        const float f = 1000.0f;
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

        glUniform3fv(glGetUniformLocation(meshShaderProgram, "lightPosition"), 1, lightPosition.data());
        checkGLErrors();

        glUniform3fv(glGetUniformLocation(meshShaderProgram, "viewerPosition"), 1, viewerPosition.data());
        checkGLErrors();


        //////////////////////
        //Render
        glBindVertexArray(vao);
        checkGLErrors();
        // draw points 0-3 from the currently bound VAO with current in-use shader
        //glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glDrawElements(GL_TRIANGLES, numFaces*3, GL_UNSIGNED_INT, 0);
        checkGLErrors();

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
        
        
        theta += 0.01f;
        frameIndex++;
    }

}
