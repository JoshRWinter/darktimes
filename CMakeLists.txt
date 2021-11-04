cmake_minimum_required(VERSION 3.0)
project(Darktimes)
add_subdirectory(win)

set(executable Darktimes)

set(sources
	src/main.cpp

	src/darktimes.hpp

	src/render/renderer.cpp
	src/render/renderer.hpp

	src/sim/simulation.cpp
	src/sim/simulation.hpp

	src/sim/levelmanager.cpp
	src/sim/levelmanager.hpp

	src/syncobject.hpp
)

if(WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
	add_executable(${executable} WIN32 ${sources})
else()
	add_executable(${executable} ${sources})
endif()

target_link_libraries(${executable} win)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${executable} PRIVATE -fno-exceptions)
elseif(WIN32)
	# /w3 is too aggressive
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif()
endif()
