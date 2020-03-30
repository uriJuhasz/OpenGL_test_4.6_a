set (targetName "OpenGLBackend")

add_library( "${targetName}" "")

add_compile_definitions(GLEW_STATIC)

set (GLEW_USE_STATIC_LIBS ON)

set (CMAKE_GENERATOR_PLATFORM x64)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

target_link_libraries("${targetName}"
	PRIVATE
#	${GLEW_STATIC_LIBRARIES}
	GLEW::GLEW
	glfw
	opengl32
	glu32
)


target_sources( "${targetName}"
	PRIVATE
		"OpenGLBackend.cpp"
	PUBLIC
		"OpenGLBackend.h"
)

add_subdirectory(private)


