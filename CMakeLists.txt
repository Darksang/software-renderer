# Project
cmake_minimum_required(VERSION 3.1)
project(software-renderer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Source Files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories(${INCLUDE_DIR})

set(SRC ${SRC_DIR}/main.cpp
        ${SRC_DIR}/window.cpp)

# Executable definition
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}" "${INCLUDE_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# SDL2 Windows
if (CMAKE_SIZEOF_VOID_P EQUAL 8 AND WIN32)
    target_link_libraries(${PROJECT_NAME} ${LIB_DIR}/sdl2/win64/SDL2.lib ${LIB_DIR}/sdl2/win64/SDL2main.lib)
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4 AND WIN32)
    target_link_libraries(${PROJECT_NAME} ${LIB_DIR}/sdl2/win32/SDL2.lib ${LIB_DIR}/sdl2/win32/SDL2main.lib)
endif()

# SDL2 Mac OS X
if (APPLE)
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
endif()