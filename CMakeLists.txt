cmake_minimum_required(VERSION 3.8)

project(Potato8 VERSION 1.0
                  DESCRIPTION "This will be a primitive chip8 emulator."
                  LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED MODULE)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Potato8 src/potato8.cpp src/potato8.h src/potato_ui.cpp src/potato_ui.h src/potato_font.h)

target_compile_features(Potato8 PUBLIC cxx_std_14)
target_link_libraries(Potato8 ${SDL2_LIBRARIES})
