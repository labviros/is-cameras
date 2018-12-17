include(GNUInstallDirs)

set(namespace "is-camera-drivers-interface")
set(target "${namespace}-info") 

find_package(Protobuf REQUIRED)
find_package(is-msgs REQUIRED)

#######
####
#######

find_package(is-msgs REQUIRED)
find_package(Protobuf REQUIRED)

get_target_property(Protobuf_IMPORT_DIRS is-msgs::is-msgs INTERFACE_INCLUDE_DIRECTORIES)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
PROTOBUF_GENERATE_CPP(camera_info_src camera_info_hdr camera-info.proto)

add_library(${target} ${camera_info_src} ${camera_info_hdr})

# compile options
set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)

# link dependencies
target_link_libraries(
  ${target}
 PUBLIC
  ${PROTOBUF_LIBRARIES}
)

# header dependencies
target_include_directories(
  ${target}
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
 PUBLIC
  ${PROTOBUF_INCLUDE_DIRS}
)

install(FILES ${interfaces} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${include_dir})

set(export_targets      ${target}Targets)
set(export_targets_file ${export_targets}.cmake)
set(export_namespace    ${namespace}::)
set(export_destination  ${CMAKE_INSTALL_LIBDIR}/cmake/${target})
set(export_config_file  ${target}Config.cmake)

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