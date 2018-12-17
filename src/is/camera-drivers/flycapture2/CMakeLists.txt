include(GNUInstallDirs)

set(namespace "is-camera-drivers")
set(target "${namespace}-flycapture2")

list(APPEND interfaces
  "driver.hpp"
  "internal/nodes.hpp"
  "internal/info.hpp"
)

list(APPEND sources 
  "driver.cpp"
  "internal/nodes.cpp"
  "internal/info.cpp"
  ${interfaces}
)

#######
####
#######

add_library(${target} ${sources})

# compile options
set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)

find_package(is-msgs REQUIRED)
find_package(is-wire REQUIRED is-wire-core)
find_package(opencv REQUIRED)

find_path(FLYCAPTURE_INCLUDE_DIRS 
 NAMES 
  FlyCapture2.h
 PATH_SUFFIXES 
  flycapture
)
message("!! FLYCAPTURE HEADERS=${FLYCAPTURE_INCLUDE_DIRS}") 

find_library(FLYCAPTURE_LIBRARY 
 NAMES 
  flycapture
)
message("!! FLYCAPTURE LIBRARY=${FLYCAPTURE_LIBRARY}") 

# link dependencies
target_link_libraries(
  ${target}
 PRIVATE
  opencv::opencv
 PUBLIC
  ${FLYCAPTURE_LIBRARY}
  is-wire::is-wire
  is-msgs::is-msgs
  is-camera-drivers::is-camera-drivers-interface
  is-camera-drivers::is-camera-drivers-utils
)

# header dependencies
target_include_directories(
  ${target}
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
  ${FLYCAPTURE_INCLUDE_DIRS}
)

set(export_targets      ${target}Targets)
set(export_targets_file ${export_targets}.cmake)
set(export_namespace    ${namespace}::)
set(export_destination  ${CMAKE_INSTALL_LIBDIR}/cmake/${target})
set(export_config_file  ${target}Config.cmake)

# install artifacts
install(FILES ${interfaces} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${include_dir})
install(
  TARGETS   ${target}
  EXPORT    ${export_targets}
  LIBRARY   DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE   DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME   DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# install export target
install(
  EXPORT      ${export_targets}
  FILE        ${export_targets_file}
  NAMESPACE   ${export_namespace}
  DESTINATION ${export_destination}
)

# install export config
install(FILES ${export_config_file} DESTINATION ${export_destination})

# create library alias (less error prone to typos)
set(target_alias ${export_namespace}${target})
add_library(${target_alias} ALIAS ${target})