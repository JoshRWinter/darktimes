cmake_minimum_required(VERSION 3.0)
project(Darktimes)
add_subdirectory(win)
set(CMAKE_CXX_STANDARD 17)

set(executable Darktimes)

set(sources
	src/main.cpp

	src/darktimes.hpp

	src/syncobject.hpp
	src/randomnumbergenerator.cpp
	src/randomnumbergenerator.hpp

	src/render/texture.hpp
	src/render/gl/gl.cpp
	src/render/gl/gl.hpp
	src/render/gl/glrenderer.cpp
	src/render/gl/glrenderer.hpp
	src/render/gl/gluirenderer.cpp
	src/render/gl/gluirenderer.hpp
	src/render/gl/floorpass.cpp
	src/render/gl/floorpass.hpp
	src/render/gl/proppass.cpp
	src/render/gl/proppass.hpp
	src/render/gl/debugquadpass.cpp
	src/render/gl/debugquadpass.hpp

	src/sim/simulation.cpp
	src/sim/simulation.hpp
	src/sim/levelgen/levelobjects.hpp
	src/sim/levelgen/propdefs.hpp
	src/sim/levelgen/structuredefs.hpp
	src/sim/levelgen/structure.hpp
	src/sim/levelgen/structure.cpp
	src/sim/levelgen/levelmanager.cpp
	src/sim/levelgen/levelmanager.hpp

	asset/recipe
	asset/shader/wall.vert
	asset/shader/wall.frag
	asset/shader/floor.vert
	asset/shader/floor.frag
	asset/shader/prop.vert
	asset/shader/prop.frag
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
