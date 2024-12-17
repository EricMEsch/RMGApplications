cmake_minimum_required(VERSION 3.8)
project(TestDistance)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # stand-alone build.
  find_package(remage REQUIRED COMPONENTS GDML)
endif()

add_executable(TestDistance main.cc HPGeTestStand.hh HPGeTestStand.cc)
target_link_libraries(TestDistance PUBLIC RMG::remage)
