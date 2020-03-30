set (targetName "Backend")

add_library( "${targetName}" "")

target_link_libraries("${targetName}" Math)
target_link_libraries("${targetName}" Geometry)


target_sources ("${targetName}"
	PUBLIC
	BackendContext.h
	BackendViewInterface.h
	BackendWindow.h

	PRIVATE
	Backend.cpp
)

add_subdirectory(Scene)


