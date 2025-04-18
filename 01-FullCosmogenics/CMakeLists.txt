cmake_minimum_required(VERSION 3.8)
project(FullCosmogenics)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(remage REQUIRED COMPONENTS GDML)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(
  FullCosmogenics
  main.cc
  src/HardwareQEOverride.cc
  src/CustomIsotopeFilter.cc
  src/CustomMUSUNGenerator.cc
  src/CosmogenicPhysics.cc
  src/RNGCerenkov.cc
  src/RNGTrackingAction.cc
  src/CosmogenicOutputScheme.cc
  src/IsotopeOutputScheme.cc)

target_link_libraries(FullCosmogenics PUBLIC RMG::remage)

target_include_directories(FullCosmogenics PUBLIC ${PROJECT_SOURCE_DIR}/include)
