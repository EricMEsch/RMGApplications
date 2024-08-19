cmake_minimum_required(VERSION 3.8)
project(TestIsotopeFilter)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(remage)

add_executable(TestIsotopeFilter main.cc HPGeTestStand.hh HPGeTestStand.cc)
target_link_libraries(TestIsotopeFilter PUBLIC RMG::remage)
