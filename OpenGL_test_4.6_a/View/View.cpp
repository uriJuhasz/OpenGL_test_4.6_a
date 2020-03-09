#include "View.h"

#include "Scene/Camera.h"

#include "Backend/ViewInterface.h"
#include "Backend/BackendWindow.h"
#include "Backend/BackendContext.h"

//#include "OpenGLBackend/OpenGLUtilities.h"

#include <GL/glew.h>

#include <string>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class ViewImpl final : public View
{
public:
    explicit ViewImpl(BackendWindow& backendWindow);

    virtual ~ViewImpl();

    void setupScene() override;
    void renderScene() override;

    void mouseWheelCallback(const Vector2& wheelDelta) override;
    void mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos) override;

public:

    void setMesh(unique_ptr<Mesh> m) override { m_mesh = move(m); }

    unique_ptr<Mesh> m_mesh;
    unique_ptr<BackendStandardShaderProgram> m_meshShaderProgram;
    unique_ptr<BackendStandardShaderProgram> m_meshEdgeShaderProgram;
    unique_ptr<BackendShaderProgram> m_lineShaderProgram;
    unique_ptr<BackendMesh> m_backendMesh;

    array<Vector3, 2> m_meshBoundingBox;

    Vector3 m_light0Position, m_light1Position;

    Camera m_sceneCamera;

private:
    BackendWindow& m_backendWindow;
};

View* View::makeView(BackendWindow& backendWindow)
{
    return new ViewImpl(backendWindow);
}

ViewImpl::ViewImpl(BackendWindow& backendWindow)
    : m_backendWindow(backendWindow)
{
    backendWindow.registerView(this);
    backendWindow.init();
}

ViewImpl::~ViewImpl()
{
    m_backendWindow.registerView(nullptr);
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

void ViewImpl::setupScene()
{
    /////////////////////////////////////////
    /////////////////////////////////////////
    //Mesh setup
    //Vertex buffers
    const auto& mesh = *m_mesh;
    m_backendMesh.reset(m_backendWindow.makeBackendMesh(mesh));

    ////////////////////////////////////////////////////////////////
    //Shaders
    ////////////////////////////////////////////////////////////////
    auto& backendContext = m_backendWindow.getContext();
    backendContext.setShaderBasePath(shaderBasePath);
    m_meshShaderProgram = backendContext.makeStandardShaderProgram("MeshVertexShader.glsl", "MeshGeometryShader.glsl", "MeshFragmentShader.glsl", "mesh");
    m_meshEdgeShaderProgram = backendContext.makeStandardShaderProgram("EdgeVertexShader.glsl", "EdgeGeometryShader.glsl", "EdgeFragmentShader.glsl", "edge");
    m_lineShaderProgram = backendContext.makeStandardShaderProgram("LineVertexShader.glsl", "", "LineFragmentShader.glsl", "line");
    m_backendMesh->setFaceShader(m_meshShaderProgram.get());
    m_backendMesh->setEdgeShader(m_meshEdgeShaderProgram.get());


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
void ViewImpl::renderScene()
{

    const auto sceneCamera = m_sceneCamera;
    const auto viewMatrix = sceneCamera.makeViewMatrix();// (viewerPosition, target, up);

    const auto viewportDimensions = m_backendWindow.getViewportDimensions();
    const auto projectionMatrix = sceneCamera.makeProjectionMatrix(viewportDimensions[0] / viewportDimensions[1]);

    //////////////////////
    //Patch sphere
    constexpr bool renderSphere = false;
    if (renderSphere)
    {
        auto& context = m_backendWindow.getContext();
        const auto shaderProgramPtr = context.makeTessellationShaderProgram(
            "SphereTesselationVertexShader.glsl",
            "SphereTesselationControlShader2.glsl",
            "SphereTesselationEvaluationShader2.glsl",
            "",
            "SphereTesselationFragmentShader.glsl",
            "Sphere"
        );

        auto& shaderProgram = *shaderProgramPtr;

        shaderProgram.setParameter("uDetail", 10.0f);

        const auto modelMatrix = unitMatrix4x4;
        shaderProgram.setParameter("modelMatrix", modelMatrix);
        shaderProgram.setParameter("viewMatrix", viewMatrix);
        shaderProgram.setParameter("projectionMatrix", projectionMatrix);

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
        auto& context = m_backendWindow.getContext();
        const auto shaderProgramPtr = context.makeTessellationShaderProgram("BezierShaderProgram.glsl", "Bezier");
        auto& shaderProgram = *shaderProgramPtr;

        const auto numPatchVertices = 16;
        glPatchParameteri(GL_PATCH_VERTICES, numPatchVertices);
        GLuint patchVertexArrayObject = 0;
        {
            const array<Vector3, 16> patchParameters = {
                Vector3(0,2,0), Vector3(1, 1,0), Vector3(2, 1,0), Vector3(3, 2,0),
                Vector3(0,1,1), Vector3(1,-2,1), Vector3(2, 1,1), Vector3(3, 0,1),
                Vector3(0,0,2), Vector3(1, 1,2), Vector3(2, 0,2), Vector3(3,-1,2),
                Vector3(0,0,3), Vector3(1, 1,3), Vector3(2,-1,3), Vector3(3,-1,3)
            };
            glGenVertexArrays(1, &patchVertexArrayObject);
            glBindVertexArray(patchVertexArrayObject);

            //Vertex positions
            GLuint vertexBuffer = 0;
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, patchParameters.size() * sizeof(patchParameters[0]), patchParameters.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }

        const Matrix4x4 modelMatrix = makeTranslationMatrix({ -1.5f,0.0f,-1.5f });
        const Vector3 target(0.0f, 0.0f, 0.0f);

        const auto viewMatrix = sceneCamera.makeViewMatrix();

        shaderProgram.setParameter("modelMatrix", modelMatrix);
        shaderProgram.setParameter("viewMatrix", viewMatrix);
        shaderProgram.setParameter("projectionMatrix", projectionMatrix);

        const Vector3 light0Position = target + Vector3(100.0f, 0.0f, 0.0f);
        const Vector3 light1Position = target + Vector3(0.0f, -1000.0f, 0.0f);
        shaderProgram.setParameter("light0Position", m_light0Position);
        shaderProgram.setParameter("light0Color", Vector3(1.0f, 1.0f, 1.0f));
        shaderProgram.setParameter("light0SpecularExponent", 10.0f);
        shaderProgram.setParameter("light1Position", m_light1Position);
        shaderProgram.setParameter("light1Color", Vector3(1.0f, 1.0f, 1.0f));
        shaderProgram.setParameter("light1SpecularExponent", 100.0f);

        shaderProgram.setParameter("viewerPosition", sceneCamera.m_position);

        const Vector3 frontColor(1.0f, 0.0f, 1.0f);
        shaderProgram.setParameter("frontColor", frontColor);
        const Vector3 backColor(0.1f, 0.3f, 0.3f);
        shaderProgram.setParameter("backColor", backColor);

        shaderProgram.setParameter("tessellationLevel", 64.0f);

        glDisable(GL_CULL_FACE);
        constexpr bool showPatchFaces = true;
        if (showPatchFaces)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// LINE);
            shaderProgram.setParameter("edgeMode", 0);
            glDepthFunc(GL_LESS);

            glBindVertexArray(patchVertexArrayObject);
            glDrawArrays(GL_PATCHES, 0, 16);
        }
        constexpr bool showPatchEdges = true;
        if (showPatchEdges)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            shaderProgram.setParameter("edgeMode", 1);
            glLineWidth(1.0f);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0, 0.0);
            glBindVertexArray(patchVertexArrayObject);
            glDrawArrays(GL_PATCHES, 0, 16);
        }
    }
    //////////////////////
    //Render mesh
    constexpr bool renderMesh = true;
    if (renderMesh)
    {
        const auto modelMatrix = unitMatrix4x4;

        const auto& mesh = *m_mesh;
        const auto numFaces = mesh.numFaces();

        constexpr bool renderMeshFaces = true;
        if (renderMeshFaces)
        {
            auto& shaderProgram = *m_meshShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);

            shaderProgram.setParameter("light0Position", m_light0Position);
            shaderProgram.setParameter("light0Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light0SpecularExponent", 10.0f);
            shaderProgram.setParameter("light1Position", m_light1Position);
            shaderProgram.setParameter("light1Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light1SpecularExponent", 100.0f);

            shaderProgram.setParameter("viewerPosition", sceneCamera.m_position);

            m_backendMesh->render(true, false);
        }

        constexpr bool renderWireframe = true;
        if (renderWireframe)
        {
            auto& shaderProgram = *m_meshEdgeShaderProgram;
            shaderProgram.setParameter("modelMatrix", modelMatrix);
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);
            shaderProgram.setParameter("edgeColor", Vector3(1.0f, 1.0f, 1.0f));

            m_backendMesh->render(false, true);


        }
        ///////////////////////
        //Bounding box
        constexpr bool showBoundingBox = true;
        if (showBoundingBox)
        {
            auto& shaderProgram = *m_lineShaderProgram;
            shaderProgram.setParameter("modelMatrix", modelMatrix);
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);

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
                glEnd();
            }
        }
    }
}

void ViewImpl::mouseWheelCallback(const Vector2& wheelDelta)
{
    auto& sceneCamera = m_sceneCamera;
    const auto dy = wheelDelta[1];
    constexpr float factor = 1.0f;
    const auto forward = sceneCamera.m_target - sceneCamera.m_position;
    const auto distance = length(forward);
    constexpr float minimalDistance = 2.0f;
    const auto newDistance = max(distance - dy * factor, minimalDistance);
    sceneCamera.m_position = sceneCamera.m_target - normalize(forward) * newDistance;
    m_backendWindow.requestUpdate();
}
void ViewImpl::mouseMoveCallback(const Vector2& delta, const Vector2& oldPos, const Vector2& newPos)
{
    if (m_backendWindow.isMiddleMouseButtonPressed())
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
        m_backendWindow.requestUpdate();

    }
    else if (m_backendWindow.isRightMouseButtonPressed())
    {
    }
}
