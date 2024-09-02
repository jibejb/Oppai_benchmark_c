cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(Oppai_benchmark_c C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
endif()

if(MSVC)

	set(TH_POOL "winthpool")
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
	

else()
		set(TH_POOL "thpool")
endif()

file(GLOB_RECURSE C_LIST src/*.c)

if (NOT ANDROID)
	message(STATUS "Building for a non-Android platform support gui")
	find_package(OpenGL REQUIRED)
	find_package(glfw3 REQUIRED)
    	add_executable(oppai_benchmark ${C_LIST} "glad/src/glad.c" ${TH_POOL}/thpool.c)
	include_directories( SYSTEM "glad/include" ${TH_POOL})
else()
	set(ANDROID_C_FILES "")

	foreach(FILE ${C_LIST})
	    if(NOT FILE MATCHES "gui_main.c")
		    list(APPEND ANDROID_C_FILES ${FILE})
	    endif()
	endforeach()

	include_directories( SYSTEM ${TH_POOL})
	add_executable(oppai_benchmark ${ANDROID_C_FILES} ${TH_POOL}/thpool.c)
endif()

		target_include_directories(oppai_benchmark PRIVATE src/)


if(MSVC)
		add_definitions(/std:c11)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)

		find_package(unofficial-pdcurses CONFIG REQUIRED)
		target_link_libraries(oppai_benchmark unofficial::pdcurses::pdcurses)
	
		find_package(PThreads4W REQUIRED)
    	target_link_libraries(oppai_benchmark PThreads4W::PThreads4W)

else()
		add_definitions(-Wall -Wextra)
		find_package(Curses REQUIRED)
        include_directories(${CURSES_INCLUDE_DIR})
        target_link_libraries(oppai_benchmark ${CURSES_LIBRARIES})

		target_link_libraries(oppai_benchmark m)
		target_link_libraries(oppai_benchmark pthread)

endif()
if (NOT ANDROID)
	message(STATUS "Building for a non-Android platform support gui")
	include_directories(${OPENGL_INCLUDE_DIR})
	target_link_libraries(oppai_benchmark ${OPENGL_gl_LIBRARY} glfw)
endif()

