cmake_minimum_required(VERSION 3.8)
project(NeutronEnergies)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # stand-alone build.
  find_package(remage REQUIRED COMPONENTS GDML)
endif()

add_executable(NeutronEnergies main.cc CustomSteppingAction.hh CustomSteppingAction.cc)
target_link_libraries(NeutronEnergies PUBLIC RMG::remage)