include(GNUInstallDirs)

set(namespace "is-camera-drivers") 
set(target "${namespace}-interface") 

find_package(is-wire REQUIRED)
find_package(is-msgs REQUIRED)

#######
####
#######

list(APPEND interfaces
  "camera-driver.hpp"
)

add_library(${target} INTERFACE)

target_link_libraries(${target}
 INTERFACE
  is-wire::is-wire
  is-msgs::is-msgs
  is-camera-drivers-interface::is-camera-drivers-interface-info
)

# header dependencies
target_include_directories(
  ${target}
 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
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

# create library alias (less error prone to typos)
set(target_alias ${export_namespace}${target})
add_library(${target_alias} ALIAS ${target})