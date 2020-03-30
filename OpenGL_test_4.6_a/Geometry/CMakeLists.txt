set (targetName "Geometry")

add_library( "${targetName}" "")

target_link_libraries("${targetName}" Math)

target_include_directories("${targetName}" PUBLIC ${root_SOURCE_DIR})

target_sources ("${targetName}"
	PUBLIC
	"Mesh.h"
	"BezierPatch.h"

	PRIVATE
	"Mesh.cpp"
	"BezierPatch.cpp"
)

