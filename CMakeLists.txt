cmake_minimum_required(VERSION 3.0.0)
project(software-renderer VERSION 0.1.0)

include(CTest)
enable_testing()

set(SDL2_INCLUDE_DIRS "C:/Libraries/SDL-2.0.9/include")
set(SDL2_LIBRARIES "C:/Libraries/SDL-2.0.9/lib/x86")

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(software-renderer main.cpp)

target_link_libraries(software-renderer C:/Libraries/SDL-2.0.9/lib/x86/SDL2.lib C:/Libraries/SDL-2.0.9/lib/x86/SDL2main.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
