set (targetName "View")

add_library( "${targetName}" "")

target_sources( "${targetName}"
	PRIVATE
		"View.cpp"
	PUBLIC
		"View.h"
)

target_link_libraries("${targetName}" Geometry Style Backend)
