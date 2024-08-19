cmake_minimum_required(VERSION 3.8)
project(FullCosmogenics)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(remage REQUIRED COMPONENTS GDML)

add_executable(FullCosmogenics main.cc)
target_link_libraries(FullCosmogenics PUBLIC RMG::remage)