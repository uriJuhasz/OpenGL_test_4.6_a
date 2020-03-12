#include "IO/MeshLoader.h"

#include "Utilities/Exception.h"

#include "OpenGLBackend/OpenGLBackend.h"

#include "View/View.h"

#include "WindowsDebugOutputRedirect.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::unique_ptr;

int main()
{
    WindowsDebugOutputRedirect::redirect();
    cout << "Start" << endl;
    cout << endl;
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\Cat_v1_L2.123c6a1c5523-ac23-407e-9fbb-d0649ffb5bcb\12161_Cat_v1_L2.obj)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\90-3ds\3ds\Dragon 2.5_3ds.3ds)";
    const auto fileName = R"(C:\Users\rossd\Downloads\BabyYoda\files\Baby_Yoda.obj)";

//   const auto fileName = R"(C:\Users\rossd\Downloads\Scorpio N05808.3ds)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\fc6bdb2aea4b58c23a3e8d4e87fba763\Elephant N090813.3DS)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\a8cfcfd0082c61bad7aa4fbd1c57a277\Ship hms victory frigate nelson N270214.3DS)";
//    const auto fileName = R"(C:\Users\rossd\Downloads\3d\e6eadc4ff882b84784dd133168c1f099\Autogenerator BelMag N170211.3DS)";
    
    try {
        auto& context = OpenGLBackend::getContext();
        
        const unique_ptr<BackendWindow> windowPtr(context.createWindow());
        auto& window = *windowPtr;
        
        const auto viewPtr = View::makeView(window);
        auto& view = *viewPtr;

        cout << " Loading mesh: " << fileName << endl;
        auto meshPtr = MeshLoader::loadMesh(fileName);
        view.setMesh(move(meshPtr));

        window.eventLoop();
    }
    catch (const Exception & e)
    {
        cerr << "  Error: " << e.getMessage();
    }

    cout << "end" << endl;
}


