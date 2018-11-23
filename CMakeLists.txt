cmake_minimum_required(VERSION 3.0.0)
project(software-renderer VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${software-renderer_SOURCE_DIR}/cmake")

if (WIN32)
    # TODO: Find SDL automatically in windows
    set(SDL2_INCLUDE_DIRS "C:/Libraries/SDL-2.0.9/include")
    set(SDL2_LIBRARIES "C:/Libraries/SDL-2.0.9/lib/x86")
    include_directories(${SDL2_INCLUDE_DIRS})
else ()
    find_package(SDL2 REQUIRED)
    include_directories(software-renderer ${SDL2_INCLUDE_DIR})
endif()

add_executable(software-renderer main.cpp)

if (WIN32)
    target_link_libraries(software-renderer ${SDL2_LIBRARIES}/SDL2.lib ${SDL2_LIBRARIES}/SDL2main.lib)
else ()
    target_link_libraries(software-renderer ${SDL2_LIBRARY})
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
