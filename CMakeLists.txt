# Project
cmake_minimum_required(VERSION 3.1)
project(software-renderer)

# Source Files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories(${INCLUDE_DIR})

set(SRC ${SRC_DIR}/main.cpp)

# Executable definition
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}" "${INCLUDE_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# SDL2
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${LIB_DIR}/sdl2/win32/SDL2.lib ${LIB_DIR}/sdl2/win32/SDL2main.lib)
endif()