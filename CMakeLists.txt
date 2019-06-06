cmake_minimum_required(VERSION 3.8)

project(Potato8 VERSION 1.0
                  DESCRIPTION "This will be a primitive chip8 emulator."
                  LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

find_package(SDL2 REQUIRED MODULE)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Potato8 
src/potato8.cpp 
src/potato_ui.cpp 
src/potato_ops.cpp
src/potato8.h 
src/potato_ui.h 
src/potato_ops.h
src/potato_font.h)

target_compile_features(Potato8 PUBLIC cxx_std_14)
target_link_libraries(Potato8 ${SDL2_LIBRARIES})
target_include_directories(Potato8 PUBLIC ext)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")