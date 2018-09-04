cmake_minimum_required(VERSION 3.10)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_paths.cmake)
  include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

add_subdirectory(msgs)
add_subdirectory(drivers)
add_subdirectory(gateway)

######
#### Build Proto Options
######

find_package(Protobuf REQUIRED)
find_package(is-msgs REQUIRED)
get_target_property(Protobuf_IMPORT_DIRS is-msgs::is-msgs INTERFACE_INCLUDE_DIRECTORIES)

PROTOBUF_GENERATE_CPP(src hdr options.proto)
list(APPEND sources ${src} ${hdr})
add_library(options ${sources})
set_property(TARGET options PROPERTY CXX_STANDARD 11)

# link dependencies
target_link_libraries(
  options
 PUBLIC
  is-msgs::is-msgs
  ${PROTOBUF_LIBRARIES} 
)

# header dependencies
target_include_directories(
  options
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
 PUBLIC
  ${PROTOBUF_INCLUDE_DIRS} 
)

######
#### Build Camera Gateway
######

find_package(opencv)

add_executable(service.bin service.cpp)

target_link_libraries(
  service.bin
 PRIVATE
  opencv::opencv
  options
  is-cameras::is-cameras-gateway
  is-cameras::is-cameras-drivers-spinnaker
  is-cameras::is-cameras-drivers-flycapture2
)

set_property(TARGET service.bin PROPERTY CXX_STANDARD 14)