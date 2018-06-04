include(CMakeFindDependencyMacro)

find_dependency(is-msgs)
find_dependency(is-cameras-msgs)

include("${CMAKE_CURRENT_LIST_DIR}/is-cameras-driversTargets.cmake")