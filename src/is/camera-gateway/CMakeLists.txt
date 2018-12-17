include(GNUInstallDirs)

set(target "service.bin")

find_package(is-wire REQUIRED is-wire-core)
find_package(is-msgs REQUIRED)
find_package(Protobuf REQUIRED)
find_package(zipkin-cpp-opentracing REQUIRED)
find_package(opencv REQUIRED)

get_target_property(Protobuf_IMPORT_DIRS is-msgs::is-msgs INTERFACE_INCLUDE_DIRECTORIES)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
PROTOBUF_GENERATE_CPP(options_src options_hdr conf/options.proto)

#######
####
#######

add_executable(${target} 
  "service.cpp"
  "camera-gateway.cpp"
  "camera-gateway.hpp"
  ${options_src}
  ${options_hdr}
)

# compile options
set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)

# link dependencies
target_link_libraries(
  ${target}
 PUBLIC
  is-msgs::is-msgs
  is-wire::is-wire
  zipkin-cpp-opentracing::zipkin-cpp-opentracing
  opencv::opencv
  # flycapture2 and spinnaker drivers must be placed in this order
  is-camera-drivers::is-camera-drivers-flycapture2
  is-camera-drivers::is-camera-drivers-spinnaker
)

# header dependencies
target_include_directories(
  ${target}
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
)