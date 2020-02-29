#include <iostream>
#include <array>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

void glfwErrorCallback(int error, const char* description)
{
    cerr << " GLFW error: " << description << endl;
}

void testOpenGL0(GLFWwindow* const window);

int main()
{
    cout << "start" << endl;
    if (glfwInit())
    {
        glfwSetErrorCallback(glfwErrorCallback);
        if (GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGLTest", nullptr,nullptr))
        {
            glfwMakeContextCurrent(window);
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
                cout << "  OpenGL renderer: " << glGetString(GL_RENDERER)  << endl;
                cout << "  OpenGL version : " << glGetString(GL_VERSION)   << endl;
                cout << "  glsl   version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

                testOpenGL0(window);
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
        cerr << " GLFW initialization failed" << endl;
    }

    cout << "end" << endl;
}


//////////////////////////////////////////////////////////////////////
template<unsigned int D> class Vector final
{
public:
    Vector(const float x, const float y, const float z) : m_value{ x,y,z } 
    {
        static_assert(D == 3);
    }
    Vector(const float x, const float y, const float z, const float w) : m_value{ x,y,z,w }
    {
        static_assert(D == 4);
    }
    const float& operator[](const int i) const { return m_value[i]; }
          float& operator[](const int i)       { return m_value[i]; }

    const float* data() const { return m_value.data(); }
          float* data()       { return m_value.data(); }
private:
    array<float, D> m_value;
};
typedef Vector<3> Vector3;
typedef Vector<4> Vector4;

const array<Vector3,4> vertices = {
   Vector3(-1.0f,  -1.0f,  0.0f),
   Vector3( 1.0f,  -1.0f,  0.0f),
   Vector3( 1.0f,   1.0f,  0.0f),
   Vector3(-1.0f,   1.0f,  0.0f)
};

const array<Vector3, 4> normals = {
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f),
   Vector3(0.0f,  0.0f, -1.0f)
};

typedef array<GLuint, 3> Triangle;
array<Triangle, 2> faces = {
    Triangle{0, 1, 2},
    Triangle{0, 2, 3}
};
constexpr const char* vertex_shader = R"(
#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 transformation;

out vec3 fragmentPosition;
out vec3 fragmentNormal;

void main() {
  vec4 pos4 = transformation * vec4(position, 1.0);
  gl_Position = pos4;
  fragmentPosition = pos4.xyz;
  fragmentNormal = normal;
})";

constexpr const char* fragment_shader = R"(
#version 400

in vec3 fragmentPosition;
in vec3 fragmentNormal;

uniform vec3 lightPosition;
uniform vec4 objectColor;

out vec4 frag_color;

void main() {
   vec3 lightDirection = (fragmentPosition-lightPosition);
   float diffuseLight = -dot(normalize(lightDirection),normalize(fragmentNormal));
   frag_color = vec4(diffuseLight * objectColor.xyz,objectColor.w);
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
void testOpenGL0(GLFWwindow* const window)
{
    glEnable(GL_DEBUG_OUTPUT);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    //Vertex buffers
    const int numVertices = static_cast<int>(vertices.size());
    
    GLuint vertexBuffer = 0;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

    checkGLErrors();
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);

    checkGLErrors();

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
    checkGLErrors();

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, nullptr);

    checkGLErrors();

    const int numFaces = static_cast<int>(faces.size());
    GLuint fao;
    glGenBuffers(1, &fao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fao);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces*sizeof(faces[0]), faces.data(), GL_STATIC_DRAW);

    checkGLErrors();

    //Shaders
    const auto vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    checkShaderErrors("Vertex", vs);

    checkGLErrors();

    const auto fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    checkShaderErrors("Fragment", fs);

    checkGLErrors();

    const auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);

    checkGLErrors();

    float theta = 0;
    while (!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        // Set the projection matrix in the vertex shader.
        const auto cost = cosf(theta);
        const auto sint = sinf(theta);
        array<float, 16> matrix =
        {
             cost, sint, 0.0f, 0.0f,
            -sint, cost, 0.0f, 0.0f,
             0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f,
        };

        checkGLErrors();

        const auto matrixLocation = glGetUniformLocation(shaderProgram, "transformation");
        glUniformMatrix4fv(matrixLocation, 1, false, matrix.data());

        checkGLErrors();

        const auto uniformLocation_lightPosition = glGetUniformLocation(shaderProgram, "lightPosition");
        const Vector3 lightPosition(0.0f, 0.0f, -1.0f);
        glUniform3fv(uniformLocation_lightPosition, 1, lightPosition.data());

        checkGLErrors();

        const auto uniformLocation_objectColor = glGetUniformLocation(shaderProgram, "objectColor");
        const Vector4 objectColor(0.5f, 0.0f, 1.0f,1.0f);
        glUniform4fv(uniformLocation_objectColor, 1, objectColor.data());

        checkGLErrors();

        glBindVertexArray(vao);
        checkGLErrors();
        // draw points 0-3 from the currently bound VAO with current in-use shader
        //glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        checkGLErrors();
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
    }

}
