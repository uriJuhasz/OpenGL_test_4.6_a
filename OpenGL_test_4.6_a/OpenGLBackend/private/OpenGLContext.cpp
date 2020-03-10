#include "OpenGLContext.h"

#include "OpenGLWindow.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLBackend/OpenGLUtilities.h"
//#include "OpenGLTesselationShaderProgram.h"

#include "Utilities/Exception.h"

#pragma warning (push,0)
#include <GLFW/glfw3.h>
#pragma warning (pop)


#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

string getShaderTypeSuffixString(const GLenum shaderType)
{
    return string(
        (shaderType == GL_VERTEX_SHADER) ? "VS" :
        (shaderType == GL_TESS_CONTROL_SHADER) ? "TCS" :
        (shaderType == GL_TESS_EVALUATION_SHADER) ? "TES" :
        (shaderType == GL_GEOMETRY_SHADER) ? "GS" :
        (shaderType == GL_FRAGMENT_SHADER) ? "FS" :
        "");
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



GLuint makeSingleShaderCC(const GLenum  shaderType, const string& shaderSource)
{
    const auto ccString = string(
        (shaderType == GL_VERTEX_SHADER) ? "VS" :
        (shaderType == GL_TESS_CONTROL_SHADER) ? "TCS" :
        (shaderType == GL_TESS_EVALUATION_SHADER) ? "TES" :
        (shaderType == GL_GEOMETRY_SHADER) ? "GS" :
        (shaderType == GL_FRAGMENT_SHADER) ? "FS" :
        "");
    const auto defineName = "COMPILING_" + ccString;
    if (shaderSource.find(defineName) != std::string::npos)
    {
        const auto shader = glCreateShader(shaderType);
        const auto defineString = "#version 430\n#define COMPILING_" + ccString + "\n";
        array<const char*, 2> ptrs = {
            defineString.c_str(),
            shaderSource.c_str()
        };
        glShaderSource(shader, 2, ptrs.data(), nullptr);
        glCompileShader(shader);
        glsCheckShaderErrors(ccString, shader);

        glsCheckErrors();
        return shader;
    }
    else
    {
        return 0;
    }
}
void makeAndAttachSingleShaderCC(const GLuint shaderProgramID, const GLenum shaderType, const string& shaderSource)
{
    const auto shaderID = makeSingleShaderCC(shaderType, shaderSource);
    if (shaderID!=0)
        glAttachShader(shaderProgramID, shaderID);
}


class GLFWContext;

class OpenGLWindowImpl;
class GLFWContext final : public OpenGLContext
{
private:
    GLFWContext(const GLFWContext&) = delete;
    GLFWContext(GLFWContext&&) = delete;
    GLFWContext& operator=(const GLFWContext&) = delete;
    GLFWContext& operator=(const GLFWContext&&) = delete;

public:
    GLFWContext()
    {
        m_valid = glfwInit();
        if (m_valid)
        {
            glfwSetErrorCallback(glfwErrorCallback);
        }
        else
        {
            const char* errorMessageC;
            glfwGetError(&errorMessageC);
            const auto errorMessage = errorMessageC ? errorMessageC : "Unknown error";
            throw new Exception(string("Could not initialize GLFW : ") + errorMessage);
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

    operator bool() const override { return m_valid; }

public:
    BackendWindow* createWindow() override
    {
        return OpenGLWindow::make(*this);
    }

public:
    string m_shaderBasePath;
    void setShaderBasePath(const string& path) override
    {
        m_shaderBasePath = path;
    }
    std::unique_ptr<BackendStandardShaderProgram> makeStandardShaderProgram(
        const string& fileName,
        const string& title) override
    {
        const auto shaderSource = loadShader(m_shaderBasePath + fileName);
        const auto shaderProgram = glCreateProgram();

        makeAndAttachSingleShaderCC(shaderProgram, GL_VERTEX_SHADER, shaderSource);
        makeAndAttachSingleShaderCC(shaderProgram, GL_GEOMETRY_SHADER, shaderSource);
        makeAndAttachSingleShaderCC(shaderProgram, GL_FRAGMENT_SHADER, shaderSource);
        glLinkProgram(shaderProgram);
        glsCheckShaderProgramErrors(title, shaderProgram);

        glsCheckErrors();

        std::cerr << "  Shader " << title << " : " << shaderProgram << endl;
        return make_unique<OpenGLStandardShaderProgram>(shaderProgram);
    }
    std::unique_ptr<BackendTesselationShaderProgram> makeTessellationShaderProgram(
        const string& fileName,
        const string& title) override
    {
        const auto shaderSource = loadShader(m_shaderBasePath + fileName);
        const auto shaderProgram = glCreateProgram();

        makeAndAttachSingleShaderCC(shaderProgram, GL_VERTEX_SHADER, shaderSource);
        makeAndAttachSingleShaderCC(shaderProgram, GL_TESS_CONTROL_SHADER, shaderSource);
        makeAndAttachSingleShaderCC(shaderProgram, GL_TESS_EVALUATION_SHADER, shaderSource);
        makeAndAttachSingleShaderCC(shaderProgram, GL_GEOMETRY_SHADER, shaderSource);
        makeAndAttachSingleShaderCC(shaderProgram, GL_FRAGMENT_SHADER, shaderSource);
        glLinkProgram(shaderProgram);
        glsCheckShaderProgramErrors(title, shaderProgram);

        glsCheckErrors();

        return make_unique<OpenGLTessellationShaderProgram>(shaderProgram);
    }

    GLuint makeSingleShader(const GLenum  shaderType, const string& shaderPath, const string& title)
    {
        const auto shader = glCreateShader(shaderType);
        const auto shaderSource = loadShader(m_shaderBasePath + shaderPath);
        const auto shaderSourcePtr = shaderSource.c_str();
        glShaderSource(shader, 1, &shaderSourcePtr, nullptr);
        glCompileShader(shader);
        glsCheckShaderErrors(title, shader);

        glsCheckErrors();
        return shader;
    }
    void makeAndAttachShader(const GLuint shaderProgram, const GLenum shaderType, const string& shaderFileName, const string& title)
    {
        if (!shaderFileName.empty())
        {
            glAttachShader(shaderProgram, makeSingleShader(shaderType, shaderFileName, title + "_" + getShaderTypeSuffixString(shaderType)));
        }
    }

private:
    static void glfwErrorCallback(int error, const char* description)
    {
        cerr << " GLFW error: " << description << endl;
    }

private:
    bool m_valid = false;
};

OpenGLContext* OpenGLContext::make()
{
    return new GLFWContext();
}
