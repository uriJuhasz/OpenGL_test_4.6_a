#include <iostream>
#include <array>

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
        if (GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGLTest", nullptr,nullptr))
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
class Vertex3 final : public array<float, 3>
{
public:
    typedef array<float, 3> Base;
    using Base::Base;

    Vertex3(const float x, const float y, const float z) : Base{ x,y,z } {}
};

const array<Vertex3,3> vertices = {
   Vertex3(0.0f,  0.5f,  0.0f),
   Vertex3(0.5f, -0.5f,  0.0f),
   Vertex3(-0.5f, -0.5f,  0.0f)
};

constexpr const char* vertex_shader =
"#version 400\n"
"in vec3 vp;"
"void main() {"
"  gl_Position = vec4(vp, 1.0);"
"}";

constexpr const char* fragment_shader =
"#version 400\n"
"out vec4 frag_colour;"
"void main() {"
"  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
"}";


void testOpenGL0(GLFWwindow* const window)
{
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    //Vertex buffers
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //Shaders
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    while (!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // update other events like input handling 
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }
}
