cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(Oppai_benchmark_c C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_EXPORT_COMPILE_COMMANDS True)
endif()

file(GLOB_RECURSE C_LIST src/*.c)
add_executable(oppai_benchmark ${C_LIST} "glad/src/glad.c")
include_directories( SYSTEM "glad/include" )
target_include_directories(oppai_benchmark PRIVATE src/)

if(WIN32 OR MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_subdirectory(pdcurses)
        target_link_libraries(oppai_benchmark
                PRIVATE
                PDcurses
        )
else()
        add_definitions(-Wall -Wextra)
        find_package(Curses REQUIRED)
        include_directories(${CURSES_INCLUDE_DIR})
        target_link_libraries(oppai_benchmark ${CURSES_LIBRARIES})
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})

target_link_libraries(oppai_benchmark ${OPENGL_gl_LIBRARY} glfw)
target_link_libraries(oppai_benchmark m)
target_link_libraries(oppai_benchmark pthread)
