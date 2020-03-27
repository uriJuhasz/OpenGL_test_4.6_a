#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpenGLScene.h"

#include "OpenGLUtilities.h"

#include "Instances/OpenGLMeshInstance.h"
#include "Instances/OpenGLBezierPatchInstance.h"
#include "Instances/OpenGLSphere.h"

#include "Primitives/OpenGLMeshPrimitive.h"
#include "Primitives/OpenGLBezierPatchPrimitive.h"

#include "Geometry/Mesh.h"
#include "Geometry/BezierPatch.h"

#include "Utilities/Misc.h"

#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

using std::vector;
using std::array;
using std::string;
using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

using std::to_string;
using std::ifstream;
using std::to_string;
using std::endl;
using std::stringstream;
static const string c_shaderBasePath = R"(C:\Users\rossd\source\repos\OpenGL_test_4.6_a\OpenGL_test_4.6_a\OpenGLBackend\private\shaders\)";

class OpenGLSceneImpl final
    : public OpenGLScene
{
public:
    explicit OpenGLSceneImpl(OpenGLWindow& window)
        : m_window(window)
    {
        loadShaders();
    }

private:
    const Camera& getCamera() const override
    {
        return m_sceneCamera;
    }
    void setCamera(const Camera& camera) override
    {
        m_sceneCamera = camera;
    }

private:
    OpenGLWindow& getBackendWindow() { return m_window; }

private:
    template<class O, class ...Args> O& addObject(const O*, Args... args)
    {
        auto ptr = make_unique<O>(args...);
        auto& ref = *ptr;
        m_sceneObjects.emplace_back(move(ptr));
        return ref;
    }
    OpenGLMeshInstance& addMesh(const shared_ptr<const Mesh>& mesh) override
    {
        const auto meshPrimitivePtr = m_meshPrimitives.emplace_back(make_unique<OpenGLMeshPrimitive>(*this, mesh)).get();
        return addObject((OpenGLMeshInstance*)nullptr, std::ref(*meshPrimitivePtr));
    }
    OpenGLMeshInstance& makeInstance(const OpenGLMeshInstance& originalInstance) override
    {
        return addObject((OpenGLMeshInstance*)nullptr,originalInstance.getPrimitive());
    }
    OpenGLBezierPatchInstance& makeInstance(const OpenGLBezierPatchInstance& originalInstance) override
    {
        return addObject((OpenGLBezierPatchInstance*)nullptr, originalInstance.getPrimitive());
    }



private:
    OpenGLBezierPatchInstance& addBezierPatch(const shared_ptr<const BezierPatch>& bezierPatchPtr) override
    {
        const auto& patchPrimitive = *m_bezierPatchPrimitives.emplace_back(make_unique<OpenGLBezierPatchPrimitive>(*this, bezierPatchPtr));
        return addObject((OpenGLBezierPatchInstance*)nullptr, std::ref(patchPrimitive));
    }
    OpenGLSphere& addSphere(const Vector3& center, const float radius) override
    {
        auto& sphere = addObject((OpenGLSphere*)nullptr, std::ref(*this), radius);
        sphere.setModelMatrix(makeTranslationMatrix(center));
        return sphere;
    }

public:
    PointLight& addPointLight() override
    {
        return *m_pointLights.emplace_back(make_unique<PointLight>());
    }

private:
    OpenGLWindow& m_window;
    Camera m_sceneCamera;
    vector<unique_ptr<OpenGLGraphicObject>> m_sceneObjects;

private:
    vector<unique_ptr<PointLight>> m_pointLights;

    vector<unique_ptr<OpenGLMeshPrimitive>> m_meshPrimitives;
    vector<unique_ptr<OpenGLBezierPatchPrimitive>> m_bezierPatchPrimitives;

private:
    void loadShaders();

    std::unique_ptr<OpenGLStandardShaderProgram> makeStandardShaderProgram(
        const string& fileName,
        const string& title);
    std::unique_ptr<OpenGLTessellationShaderProgram> makeTessellationShaderProgram(
        const string& fileName,
        const string& title);

    GLuint makeSingleShader(const GLenum  shaderType, const string& shaderPath, const string& title);
    void makeAndAttachShader(const GLuint shaderProgram, const GLenum shaderType, const string& shaderFileName, const string& title);
    std::unique_ptr<OpenGLStandardShaderProgram> makeStandardShaderProgramModular(
        const string& vertexShader,
        const string& geometryShader,
        const string& fragmentShader,
        const string& title);

public:
    OpenGLStandardShaderProgram& getPointsShader() const override
    {
        return *m_fixedColorShader;
    }

    OpenGLStandardShaderProgram& getBoundingBoxShader() const override
    {
        return *m_boundingBoxShader;
    }
    OpenGLStandardShaderProgram& getMeshFaceShader() const override
    {
        return *m_meshFaceShader;
    }
    OpenGLStandardShaderProgram& getMeshEdgeShader() const override
    {
        return *m_fixedColorShader;
    }
    OpenGLTessellationShaderProgram& getBezierPatchFaceShader() const override
    {
        return *m_bezierFaceShader;
    }
    OpenGLTessellationShaderProgram& getBezierPatchEdgeShader() const override
    {
        return *m_bezierEdgeShader;
    }

    OpenGLTessellationShaderProgram& getSphereEdgeShader() const override
    {
        return *m_sphereEdgeShader;
    }
    OpenGLTessellationShaderProgram& getSphereFaceShader() const override
    {
        return *m_sphereFaceShader;
    }

    unique_ptr<OpenGLStandardShaderProgram> m_fixedColorShader;

    unique_ptr<OpenGLStandardShaderProgram> m_boundingBoxShader;

    unique_ptr<OpenGLStandardShaderProgram> m_meshFaceShader;

    unique_ptr<OpenGLTessellationShaderProgram> m_bezierFaceShader;
    unique_ptr<OpenGLTessellationShaderProgram> m_bezierEdgeShader;
    unique_ptr<OpenGLTessellationShaderProgram> m_sphereFaceShader;
    unique_ptr<OpenGLTessellationShaderProgram> m_sphereEdgeShader;

public:
    void render() const
    {
        const vector<OpenGLShaderProgram*> allShaders = {
            m_fixedColorShader.get(),
            m_boundingBoxShader.get(),
            m_meshFaceShader.get(),
            m_bezierFaceShader.get(),
            m_bezierEdgeShader.get(),
            m_sphereFaceShader.get(),
            m_sphereEdgeShader.get()
        };

        const auto viewportDimensions = m_window.getViewportDimensions();
        const auto viewMatrix = m_sceneCamera.makeViewMatrix();
        const auto projectionMatrix = m_sceneCamera.makeProjectionMatrix(viewportDimensions[0] / viewportDimensions[1]);

        for (OpenGLShaderProgram* shaderPtr : allShaders)
        {
            auto& shader = *shaderPtr;
            shader.setParameter("viewMatrix", viewMatrix);
            shader.setParameter("projectionMatrix", projectionMatrix);

            constexpr int maxShaderLights = 2;
            for (int lightIndex = 0; lightIndex < maxShaderLights; ++lightIndex)
            {
                const string lis = to_string(lightIndex);
                const auto& light = *m_pointLights[lightIndex];
                if (shader.hasLight(lightIndex))
                {
                    shader.setParameter("light" + lis + "Position", light.getPosition());
                    shader.setParameter("light" + lis + "Color", light.m_color.m_value);
                    shader.setParameter("light" + lis + "SpecularExponent", light.m_specularExponent);
                }
                shader.setParameterIfExists("viewerPosition", getCamera().m_position);
            }
        }

        const vector<OpenGLShaderProgram*> allAdaptiveTessellationShaders = {
            m_bezierFaceShader.get(),
            m_bezierEdgeShader.get(),
            m_sphereFaceShader.get(),
            m_sphereEdgeShader.get()
        };

        const auto pixelWidth = m_window.getFramebufferSize()[0];
        for (auto shaderPtr : allAdaptiveTessellationShaders)
            shaderPtr->setParameter("pixelWidth", pixelWidth);

        { //Smooth lines
            constexpr bool antialiasedLines = true;
            if (antialiasedLines)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glEnable(GL_LINE_SMOOTH);
                glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            }
            else
            {
                glDisable(GL_BLEND);
                glDisable(GL_POLYGON_SMOOTH);
                glDisable(GL_LINE_SMOOTH);
            }
        }

        for (auto& instancePtr : m_sceneObjects)
            instancePtr->render();
    }

private:
    string m_shaderBasePath;
    void setShaderBasePath(const string& path)
    {
        m_shaderBasePath = path;
    }

};

std::unique_ptr<OpenGLScene> makeScene(OpenGLWindow& window)
{
    return make_unique<OpenGLSceneImpl>(window);

}

///////////////////////////////////////////
//Shaders
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
    else
        throw new int();

    return r;
}

GLuint makeSingleShaderCC(const GLenum shaderType, const string& shaderSource)
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
    if (shaderID != 0)
        glAttachShader(shaderProgramID, shaderID);
}

void OpenGLSceneImpl::loadShaders()
{
    setShaderBasePath(c_shaderBasePath);

    m_fixedColorShader = makeStandardShaderProgramModular("allTranformations.vert", "", "fixedColor.frag", "points");
//    m_pointsShader = makeStandardShaderProgram("PointsShader.glsl", "points");

    m_boundingBoxShader = makeStandardShaderProgram("BoundingBoxShaderProgram.glsl", "boundingBox");

    m_meshFaceShader = makeStandardShaderProgram("MeshFaceShaderProgram.glsl", "meshFace");

    m_bezierFaceShader = makeTessellationShaderProgram("BezierFaceShaderProgram.glsl", "BezierFace");
    m_bezierEdgeShader = makeTessellationShaderProgram("BezierEdgeShaderProgram.glsl", "BezierEdge");

    m_sphereFaceShader = makeTessellationShaderProgram("SphereFaceShaderProgram.glsl", "SphereFace");
    m_sphereEdgeShader = makeTessellationShaderProgram("SphereEdgeShaderProgram.glsl", "SphereEdge");

    for (auto shaderPtr : { m_bezierFaceShader.get(), m_bezierEdgeShader.get() })
    {
        auto& shader = *shaderPtr;
        shader.setParameter("maxTessellationLevel", glsGetUInt(GL_MAX_TESS_GEN_LEVEL));
        shader.setParameter("desiredPixelsPerTriangle", 20.0f);
    }
    for (auto shaderPtr : { m_sphereFaceShader.get(), m_sphereEdgeShader.get()})
    {
        auto& shader = *shaderPtr;
        shader.setParameter("maxTessellationLevel", glsGetUInt(GL_MAX_TESS_GEN_LEVEL));
        shader.setParameter("desiredPixelsPerTriangle", 5.0f);
    }
}

std::unique_ptr<OpenGLStandardShaderProgram> OpenGLSceneImpl::makeStandardShaderProgram(
    const string& fileName,
    const string& title)
{
    glBindVertexArray(0);
    glUseProgram(0);

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
std::unique_ptr<OpenGLTessellationShaderProgram> OpenGLSceneImpl::makeTessellationShaderProgram(
    const string& fileName,
    const string& title)
{
    const auto shaderSource = loadShader(m_shaderBasePath + fileName);
    const auto shaderProgramID = glCreateProgram();

    makeAndAttachSingleShaderCC(shaderProgramID, GL_VERTEX_SHADER, shaderSource);
    makeAndAttachSingleShaderCC(shaderProgramID, GL_TESS_CONTROL_SHADER, shaderSource);
    makeAndAttachSingleShaderCC(shaderProgramID, GL_TESS_EVALUATION_SHADER, shaderSource);
    makeAndAttachSingleShaderCC(shaderProgramID, GL_GEOMETRY_SHADER, shaderSource);
    makeAndAttachSingleShaderCC(shaderProgramID, GL_FRAGMENT_SHADER, shaderSource);
    glLinkProgram(shaderProgramID);
    glsCheckShaderProgramErrors(title, shaderProgramID);

    glsCheckErrors();

    std::cerr << "  Shader " << title << " : " << shaderProgramID << endl;

    return make_unique<OpenGLTessellationShaderProgram>(shaderProgramID);
}

GLuint OpenGLSceneImpl::makeSingleShader(const GLenum shaderType, const string& shaderPath, const string& title)
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
void OpenGLSceneImpl::makeAndAttachShader(const GLuint shaderProgram, const GLenum shaderType, const string& shaderFileName, const string& title)
{
    if (!shaderFileName.empty())
    {
        glAttachShader(shaderProgram, makeSingleShader(shaderType, shaderFileName, title + "_" + getShaderTypeSuffixString(shaderType)));
    }
}

std::unique_ptr<OpenGLStandardShaderProgram> OpenGLSceneImpl::makeStandardShaderProgramModular(
    const string& vertexShader,
    const string& geometryShader,
    const string& fragmentShader,
    const string& title)
{
    glBindVertexArray(0);
    glUseProgram(0);

    const auto shaderProgram = glCreateProgram();

    makeAndAttachShader(shaderProgram, GL_VERTEX_SHADER, vertexShader, title);
    makeAndAttachShader(shaderProgram, GL_GEOMETRY_SHADER, geometryShader, title);
    makeAndAttachShader(shaderProgram, GL_FRAGMENT_SHADER, fragmentShader, title);
    glLinkProgram(shaderProgram);
    glsCheckShaderProgramErrors(title, shaderProgram);

    glsCheckErrors();

    return make_unique<OpenGLStandardShaderProgram>(shaderProgram);
}

std::unique_ptr<OpenGLScene> OpenGLScene::makeScene(OpenGLWindow& window)
{
    return make_unique<OpenGLSceneImpl>(window);
}
