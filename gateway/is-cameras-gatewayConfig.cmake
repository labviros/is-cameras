include(CMakeFindDependencyMacro)

find_dependency(is-wire-core)
find_dependency(is-msgs)
find_dependency(is-cameras-drivers)

include("${CMAKE_CURRENT_LIST_DIR}/is-cameras-gatewayTargets.cmake")