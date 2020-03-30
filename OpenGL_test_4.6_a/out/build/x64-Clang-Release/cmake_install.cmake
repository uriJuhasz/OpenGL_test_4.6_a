# Install script for directory: C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/install/x64-Clang-Release")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Backend/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Documentation/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Geometry/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/IO/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Main/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Math/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/OpenGLBackend/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Style/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/Utilities/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/View/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/WindowsBackend/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/rossd/source/repos/OpenGL_test_4.6_a/OpenGL_test_4.6_a/out/build/x64-Clang-Release/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
