project(SpringMapConvNG)
cmake_minimum_required(VERSION 2.6)
#find_package(Qt4 REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SpringMapConvNG_SRCS CRC.cpp Image.cpp SMFMap.cpp main.cpp TileStorage.cpp)
#qt4_automoc(${SpringMapConvNG_SRCS})
add_executable(SpringMapConvNG ${SpringMapConvNG_SRCS})
target_link_libraries(SpringMapConvNG IL ILU )
