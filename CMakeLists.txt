cmake_minimum_required(VERSION 3.10)
include(GNUInstallDirs)

project(is-cameras CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("./src/is/camera-drivers")
add_subdirectory("./src/is/camera-gateway")