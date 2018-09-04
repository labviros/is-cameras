include(CMakeFindDependencyMacro)

find_dependency(is-wire-core)
find_dependency(is-msgs)
find_dependency(is-cameras-drivers)
find_dependency(OpenCV)

include("${CMAKE_CURRENT_LIST_DIR}/is-cameras-drivers-spinnakerTargets.cmake")