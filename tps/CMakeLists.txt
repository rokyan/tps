# CMakeList.txt : CMake project for tps, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories ("${CMAKE_SOURCE_DIR}/tps/src")
include_directories ("${CMAKE_SOURCE_DIR}/tps/demo")

file (GLOB_RECURSE TPS_HEADERS "*.hpp")
file (GLOB_RECURSE TPS_SOURCES "*.cpp")

# Add source to this project's executable.
add_executable (tps ${TPS_HEADERS} ${TPS_SOURCES} )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property (TARGET tps PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.