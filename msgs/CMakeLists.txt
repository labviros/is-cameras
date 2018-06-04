include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Protobuf REQUIRED)
find_package(is-msgs REQUIRED)

set(author "is")
set(module "cameras")
set(module_levels "msgs")

list(APPEND protos
  "camera-info.proto"
)

#######
####
#######

string(REPLACE ";" "-" target "${author};${module};${module_levels}")
string(REPLACE ";" "/" include_dir "${author};${module};${module_levels}")
set(namespace "${author}-${module}") 

foreach(proto ${protos})
  PROTOBUF_GENERATE_CPP(src hdr ${proto})
  list(APPEND sources ${src} ${hdr})
  list(APPEND interfaces ${hdr} ${proto})
endforeach()

add_library(${target} ${sources})

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