#include "Math.h"
#include "Mesh.h"

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <initializer_list>

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

typedef array<GLuint, 3> Triangle;
array<Triangle, 2> faces = {
    Triangle{0, 1, 2},
    Triangle{0, 2, 3}
};



constexpr const char* vertex_shader = R"(
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
  fragmentNormal = (modelMatrix*vec4(normal,1)).xyz;
  fragmentUVCoord = uvCoord;
})";

constexpr const char* fragment_shader = R"(
#version 400

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec2 fragmentUVCoord;

uniform vec3 lightPosition;
uniform vec4 objectColor;

out vec4 frag_color;

void main() {
   vec3 lightDirection = (fragmentPosition-lightPosition);
   float diffuseLight = -dot(normalize(lightDirection),normalize(fragmentNormal));
   vec3 baseColor = 
            vec3(1,0,0)*(1-fragmentUVCoord.x-fragmentUVCoord.y) + 
            vec3(0,0,1)*(fragmentUVCoord.x-fragmentUVCoord.y) + 
            vec3(0,1,0)*(fragmentUVCoord.y-fragmentUVCoord.x);
   vec3 diffuseColor = /*diffuseLight **/ baseColor;//objectColor.xyz;
   const vec3 lightSpecularColor = vec3(1,1,1);
   vec3 specularColor = lightSpecularColor * 0.3/length(lightDirection);
   frag_color = vec4(diffuseColor/*+specularColor*/,objectColor.w);
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

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
    checkGLErrors();

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, textureUV.size() * sizeof(textureUV[0]), textureUV.data(), GL_STATIC_DRAW);
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
    float dist = 0;
    while (!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        checkGLErrors();

        // Set the projection matrix in the vertex shader.
        const auto cost = cosf(theta);
        const auto sint = sinf(theta);
        Matrix44 modelMatrix = 
        {
             cost, sint, 0.0f, 0.0f,
            -sint, cost, 0.0f, 0.0f,
             0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f,
        };


        //Setup the view matrix
        const auto t2 = theta * 2;
        const auto cosx = cosf(t2);
        const auto sinx = sinf(t2);
        const auto vd = 3.0f;
        const Vector3 viewer(vd * cosx, 0, vd * sinx);
        const Vector3 up(0.0f, 1.0f, 0.0f);
        const Vector3 target(0.0f, 0.0f, 0.0f);
        const auto forward = normalize(target - viewer);
        const auto right = cross(forward, up);

        Matrix44 viewMatrix =
        {
                right  [0], right  [1], right  [2], -dot(right,viewer),
                up     [0], up     [1], up     [2], -dot(up,viewer),
                forward[0], forward[1], forward[2], -dot(forward,viewer),
                0.0f,0.0f, 0.0f, 1.0f
        };
                
/*
        Matrix44 viewMatrix =
        {
             1.0f, 0.0f, 0.0f, 0.0f,//4*cost, //0.0f,
             0.0f, 1.0f, 0.0f, 0.0f, //0.0f,
             0.0f, 0.0f, 1.0f, 3.0f, //4*sint,
             0.0f, 0.0f, 0.0f, 1.0f,
        };
        */

        
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
        const float f = 100.0f;
        const float d = f - n;
        Matrix44 projectionMatrix =
        {
            2*n/w, 0.0f, 0.0f, 0.0f,
            0.0f, 2*n/h, 0.0f, 0.0f,
            0.0f, 0.0f, (f+n)/d, -2*f*n/d,
            0.0f, 0.0f, 1.0f, 0.0f
        };
/*
        const float n = 0.1f;
        const float f = 1000.0f;
        Matrix44 projectionMatrix =
        {
             2.0f * n / w, 0.0f, 0.0f, 0.0f,
             0.0f, 2.0f * n / h, 0.0f, 0.0f,
             0.0f, 0.0f, f / (f - n), -f * n / (f - n),
             0.0f, 0.0f, 0.0f, 1.0f,
        };
        */
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, false, modelMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, true, viewMatrix.data());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, true, projectionMatrix.data());
        checkGLErrors();

        const Vector3 lightPosition(0.0f, 0.0f, -1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosition"), 1, lightPosition.data());
        checkGLErrors();

        const Vector4 objectColor(0.5f, 0.0f, 1.0f,1.0f);
        glUniform4fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, objectColor.data());
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
