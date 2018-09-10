include(GNUInstallDirs)

find_package(is-msgs REQUIRED)
find_package(is-wire REQUIRED is-wire-core)
find_package(opencv)

find_path(SPINNAKER_INCLUDE_DIRS 
 NAMES 
  Spinnaker.h
 PATH_SUFFIXES 
  spinnaker
)
message("!! SPINNAKER HEADERS=${SPINNAKER_INCLUDE_DIRS}")
  
find_library(SPINNAKER_LIBRARY 
 NAMES 
  Spinnaker
)
message("!! SPINNAKER LIBRARY=${SPINNAKER_LIBRARY}")

set(author "is")
set(module "cameras")
set(module_levels "drivers" "spinnaker")

set(interfaces)
list(APPEND interfaces
"driver.hpp"
"internal/info.hpp"
"internal/nodes.hpp"
)

set(sources)
list(APPEND sources 
  "driver.cpp"
  "internal/info.cpp"
  "internal/nodes.cpp"
  ${interfaces}
)

#######
####
#######

string(REPLACE ";" "-" target "${author};${module};${module_levels}")
string(REPLACE ";" "/" include_dir "${author};${module};${module_levels}")
set(namespace "${author}-${module}") 

add_library(${target} ${sources})

# compile options
set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)

# link dependencies
target_link_libraries(
  ${target}
 PRIVATE
  opencv::opencv
 PUBLIC
  ${SPINNAKER_LIBRARY}
  is-wire::is-wire
  is-msgs::is-msgs
  is-cameras::is-cameras-drivers
)

# header dependencies
target_include_directories(
  ${target}
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
  ${SPINNAKER_INCLUDE_DIRS}
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

#####
### Tests
#####

if(enable_tests)
  enable_testing()
  find_package(GTest REQUIRED)

  function(gtest_add target source deps)
    add_executable(${target} ${source})
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)
    target_link_libraries(${target} GTest::GTest GTest::Main ${deps})
    add_test(${target} ${target}) 
  endfunction()

  foreach(test ${tests})
    get_filename_component(test_target ${test} NAME_WE)
    gtest_add(${test_target}_test ${test} ${target})
  endforeach(test)
  
endif(enable_tests)