//#include "Math.h"
#include "Geometry/Mesh.h"

#include "IO/MeshLoader.h"

#include "Scene/Scene.h"

#include "OpenGLBackend/OpenGLUtilities.h"
#include "OpenGLBackend/OpenGLBackend.h"

#include "Backend/BackendWindow.h"
#include "View/View.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

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
    auto meshPtr = MeshLoader::loadMesh(fileName);
    if (meshPtr)
    {
        auto& context = OpenGLBackend::getContext();
        if (context)
        {
            const unique_ptr<BackendWindow> window(context.createWindow());
            if (window)
            {
                const auto view = View::makeView(*window);
                view->setMesh(move(meshPtr));
                window->eventLoop();
            }
            else
            {
                cerr << " GLFW: failed to create window" << endl;
            }
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


