set (targetName "OpenGL_4_6_Test_1")

add_executable(${targetName} main.cpp)

target_link_libraries("${targetName}" OpenGLBackend View IO)


