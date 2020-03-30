cmake_minimum_required(VERSION 3.16)

project(OpenGL_4_6_Test_1 VERSION 1.0)

cmake_policy(SET CMP0012 NEW)

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()


include_directories(${root_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(Backend)
add_subdirectory(Documentation)
add_subdirectory(Geometry)
add_subdirectory(IO)
add_subdirectory(Main)
add_subdirectory(Math)
add_subdirectory(OpenGLBackend)
add_subdirectory(Style)
add_subdirectory(Utilities)
add_subdirectory(View)
add_subdirectory(WindowsBackend)
