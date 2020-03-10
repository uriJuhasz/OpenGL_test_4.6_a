#include "View.h"

#include "Scene/Camera.h"

#include "Backend/BackendViewInterface.h"
#include "Backend/BackendWindow.h"
#include "Backend/BackendContext.h"

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

public: //Mesh
    unique_ptr<Mesh> m_mesh;
    unique_ptr<BackendStandardShaderProgram> m_meshFaceShaderProgram;
    unique_ptr<BackendStandardShaderProgram> m_meshEdgeShaderProgram;
    unique_ptr<BackendShaderProgram> m_meshBoundingboxShaderProgram;
    unique_ptr<BackendMesh> m_backendMesh;

    array<Vector3, 2> m_meshBoundingBox;

public: //Bezier patch
    unique_ptr<Patch> m_bezierPatch;
    unique_ptr<BackendPatch> m_backendBezierPatch;
    unique_ptr<BackendTesselationShaderProgram> m_bezierShaderProgram;

public: //Sphere
    unique_ptr<Patch> m_spherePatch;
    unique_ptr<BackendPatch> m_backendSpherePatch;
    unique_ptr<BackendTesselationShaderProgram> m_sphereShaderProgram;

public: //Scene parameters
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
    auto& backendContext = m_backendWindow.getContext();
    backendContext.setShaderBasePath(shaderBasePath);

    /////////////////////////////////////////
    /////////////////////////////////////////
    //Setup lights
    {
        const Vector3 target(0.0f, 0.0f, 0.0f);
        m_light0Position = target + Vector3(0.0f, 100.0f, 0.0f);
        m_light1Position = target + Vector3(0.0f, -1000.0f, 0.0f);
    }

    /////////////////////////////////////////
    /////////////////////////////////////////
    //Mesh setup
    {
        const auto& mesh = *m_mesh;
        m_backendMesh.reset(m_backendWindow.makeBackendMesh(mesh));


        const auto modelMatrix = unitMatrix4x4;
        {
            m_meshFaceShaderProgram = backendContext.makeStandardShaderProgram("MeshFaceShaderProgram.glsl", "meshFace");// "MeshVertexShader.glsl", "MeshGeometryShader.glsl", "MeshFragmentShader.glsl", "mesh");
            m_backendMesh->setFaceShader(m_meshFaceShaderProgram.get());

            auto& shaderProgram = *m_meshFaceShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);

            shaderProgram.setParameter("light0Position", m_light0Position);
            shaderProgram.setParameter("light0Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light0SpecularExponent", 10.0f);
            shaderProgram.setParameter("light1Position", m_light1Position);
            shaderProgram.setParameter("light1Color", Vector3(1.0f, 1.0f, 1.0f));
            shaderProgram.setParameter("light1SpecularExponent", 100.0f);
        }
        {
            m_meshEdgeShaderProgram = backendContext.makeStandardShaderProgram("MeshEdgeShaderProgram.glsl", "meshEdge"); //"EdgeVertexShader.glsl", "EdgeGeometryShader.glsl", "EdgeFragmentShader.glsl", "edge");
            m_backendMesh->setEdgeShader(m_meshEdgeShaderProgram.get());
            auto& shaderProgram = *m_meshEdgeShaderProgram;
            const auto modelMatrix = unitMatrix4x4;

            shaderProgram.setParameter("modelMatrix", modelMatrix);

            shaderProgram.setParameter("edgeColor", Vector3(1.0f, 1.0f, 1.0f));
        }

        {
            m_meshBoundingboxShaderProgram = backendContext.makeStandardShaderProgram("BoundingBoxShaderProgram.glsl", "boundingBoxLine");
            auto& shaderProgram = *m_meshFaceShaderProgram;

            shaderProgram.setParameter("modelMatrix", modelMatrix);

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
//            const auto modelCenter = (boundingBox[0] + boundingBox[1]) / 2;
//            const auto modelRadius = length(boundingBox[1] - boundingBox[0]) * 0.5f;
        }
    }

    ////////////////////////////////////////////////////////////////
    //Bezier patch
    {
        const array<Vector3, 16> patchParameters = {
            Vector3(0,2,0), Vector3(1, 1,0), Vector3(2, 1,0), Vector3(3, 2,0),
            Vector3(0,1,1), Vector3(1,-2,1), Vector3(2, 1,1), Vector3(3, 0,1),
            Vector3(0,0,2), Vector3(1, 1,2), Vector3(2, 0,2), Vector3(3,-1,2),
            Vector3(0,0,3), Vector3(1, 1,3), Vector3(2,-1,3), Vector3(3,-1,3)
        };
        m_bezierPatch.reset(new Patch(16, make_unique<VertexArray3f>(vector<Vector3>(patchParameters.cbegin(),patchParameters.cend()))));
        m_backendBezierPatch.reset(m_backendWindow.makeBackendPatch(*m_bezierPatch));
        m_bezierShaderProgram = backendContext.makeTessellationShaderProgram("BezierShaderProgram.glsl", "Bezier");
        m_backendBezierPatch->setFaceShader(m_bezierShaderProgram.get());
        m_backendBezierPatch->setEdgeShader(m_bezierShaderProgram.get());
        auto& shaderProgram = *m_bezierShaderProgram;

        const Matrix4x4 modelMatrix = makeTranslationMatrix({ -1.5f,0.0f,-1.5f });
        shaderProgram.setParameter("modelMatrix", modelMatrix);

        const Vector3 target(0.0f, 0.0f, 0.0f);

        const Vector3 light0Position = target + Vector3(100.0f, 0.0f, 0.0f);
        const Vector3 light1Position = target + Vector3(0.0f, -1000.0f, 0.0f);
        shaderProgram.setParameter("light0Position", m_light0Position);
        shaderProgram.setParameter("light0Color", Vector3(1.0f, 1.0f, 1.0f));
        shaderProgram.setParameter("light0SpecularExponent", 10.0f);
        shaderProgram.setParameter("light1Position", m_light1Position);
        shaderProgram.setParameter("light1Color", Vector3(1.0f, 1.0f, 1.0f));
        shaderProgram.setParameter("light1SpecularExponent", 100.0f);

        const Vector3 frontColor(1.0f, 0.0f, 1.0f);
        shaderProgram.setParameter("frontColor", frontColor);
        const Vector3 backColor(0.1f, 0.3f, 0.3f);
        shaderProgram.setParameter("backColor", backColor);

        shaderProgram.setParameter("tessellationLevel", 64.0f);
    }

    ////////////////////////////////////////////////////////////////
    //Sphere
    {
        const vector<Vector4> patchParameters = {
            Vector4(20.0f, 0.0f, 0.0f, 10.0f),
            Vector4(0.0f, 0.0f, -50.0f, 30.0f)
        };
        m_spherePatch.reset(new Patch(1, make_unique<VertexArray4f>(patchParameters)));
        m_backendSpherePatch.reset(m_backendWindow.makeBackendPatch(*m_spherePatch));
        m_sphereShaderProgram = backendContext.makeTessellationShaderProgram("SphereShaderProgram.glsl", "Sphere");

        auto& shaderProgram = *m_sphereShaderProgram;

        shaderProgram.setParameter("uDetail", 30.0f);

        const auto modelMatrix = unitMatrix4x4;
        shaderProgram.setParameter("modelMatrix", modelMatrix);

        m_backendSpherePatch->setEdgeShader(m_sphereShaderProgram.get());
    }

}
void ViewImpl::renderScene()
{
    const auto sceneCamera = m_sceneCamera;
    const auto viewMatrix = sceneCamera.makeViewMatrix();// (viewerPosition, target, up);

    const auto viewportDimensions = m_backendWindow.getViewportDimensions();
    const auto projectionMatrix = sceneCamera.makeProjectionMatrix(viewportDimensions[0] / viewportDimensions[1]);

    //////////////////////
    //Patch sphere
    constexpr bool renderSphere = true;
    if (renderSphere)
    {
        auto& shaderProgram = *m_sphereShaderProgram;
        shaderProgram.setParameter("viewMatrix", viewMatrix);
        shaderProgram.setParameter("projectionMatrix", projectionMatrix);

        m_backendSpherePatch->render(false, true);
    }

    //////////////////////
    //Bezier patch
    constexpr bool showBezierPatch = true;
    if (showBezierPatch)
    {
        auto& shaderProgram = *m_bezierShaderProgram;
        const auto viewMatrix = sceneCamera.makeViewMatrix();
        shaderProgram.setParameter("viewMatrix", viewMatrix);
        shaderProgram.setParameter("projectionMatrix", projectionMatrix);

        shaderProgram.setParameter("viewerPosition", sceneCamera.m_position);

        shaderProgram.setParameter("edgeMode", 0);
        m_backendBezierPatch->render(true, false);
        shaderProgram.setParameter("edgeMode", 1);
        m_backendBezierPatch->render(false, true);
    }
    //////////////////////
    //Render mesh
    constexpr bool renderMesh = true;
    if (renderMesh)
    {
        constexpr bool renderMeshFaces = true;
        if (renderMeshFaces)
        {
            auto& shaderProgram = *m_meshFaceShaderProgram;

            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);
            shaderProgram.setParameter("viewerPosition", sceneCamera.m_position);

            m_backendMesh->render(true, false);
        }

        constexpr bool renderWireframe = true;
        if (renderWireframe)
        {
            auto& shaderProgram = *m_meshEdgeShaderProgram;
            shaderProgram.setParameter("viewMatrix", viewMatrix);
            shaderProgram.setParameter("projectionMatrix", projectionMatrix);

            m_backendMesh->render(false, true);
        }
        ///////////////////////
        //Bounding box
        constexpr bool showBoundingBox = true;
        if (showBoundingBox)
        {
            auto& shaderProgram = *m_meshBoundingboxShaderProgram;
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

            glDepthFunc(GL_LESS);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            glLineWidth(2.0f);

            glBegin(GL_LINES);
            for (int e = 0; e < 12; ++e)
            {
                const auto v0 = boundingBoxVertices[bbEdges[2 * e + 0]];
                const auto v1 = boundingBoxVertices[bbEdges[2 * e + 1]];
                glVertex3fv(v0.data());
                glVertex3fv(v1.data());
            }
            glEnd();

            glBegin(GL_LINES);
                glVertex3f(-1.0, -1.0f, -1.0f);
                glVertex3f( 1.0,  1.0f,  1.0f);
            glEnd();
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
