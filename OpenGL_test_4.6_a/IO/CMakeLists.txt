set (targetName "IO")
add_library( "${targetName}" "")

target_sources( "${targetName}"
	PRIVATE
		"MeshLoader.cpp"
	PUBLIC
		"MeshLoader.h"
)

#find_package(assimp REQUIRED)

#target_link_libraries("${targetName}" assimp::assimp)

target_include_directories("${targetName}" PRIVATE "C:/Code/assimp/assimp-5.0.1/out/install/x64-Release/include")
target_link_libraries("${targetName}" PRIVATE "C:/Code/assimp/assimp-5.0.1/out/install/x64-Release/lib/assimp-vc142-mt.lib")
