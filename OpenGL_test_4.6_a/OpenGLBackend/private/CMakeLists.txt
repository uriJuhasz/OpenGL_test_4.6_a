
target_sources( "${targetName}"
	PRIVATE
		OpenGLContext.cpp
		OpenGLWindow.cpp
		OpenGLScene.cpp
		OpenGLUtilities.cpp
		OpenGLShaderProgram.cpp

		OpenGLContext.h
		OpenGLWindow.h
		OpenGLScene.h
		OpenGLUtilities.h
		OpenGLShaderProgram.h
)

add_subdirectory(Instances)
add_subdirectory(Primitives)
add_subdirectory(Shaders)

