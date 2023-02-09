cmake_minimum_required(VERSION 3.0)
project(Darktimes)
add_subdirectory(win)
set(CMAKE_CXX_STANDARD 17)

set(executable Darktimes)

set(sources
	src/main.cpp

	src/Darktimes.hpp

	src/SyncObjectManager.hpp
	src/LevelRenderState.hpp
	src/RenderState.hpp
	src/Input.hpp
	src/RenderState.hpp
	src/RandomNumberGenerator.cpp
	src/RandomNumberGenerator.hpp

	src/render/Renderable.hpp
	src/render/Texture.hpp
	src/render/gl/GL.cpp
	src/render/gl/GL.hpp
	src/render/gl/GLRenderer.cpp
	src/render/gl/GLRenderer.hpp
	src/render/gl/GLUIRenderer.cpp
	src/render/gl/GLUIRenderer.hpp
	src/render/gl/FloorPass.cpp
	src/render/gl/FloorPass.hpp
	src/render/gl/PropPass.cpp
	src/render/gl/PropPass.hpp

	src/sim/World.hpp
	src/sim/Simulation.cpp
	src/sim/Simulation.hpp

	src/sim/Entity.hpp
	src/sim/component/Component.hpp
	src/sim/component/PhysicalComponent.hpp
	src/sim/component/RenderableComponent.hpp

	src/sim/levelgen/LevelObjects.hpp
	src/sim/levelgen/PropDefs.hpp
	src/sim/levelgen/StructureDefs.hpp
	src/sim/levelgen/Structure.hpp
	src/sim/levelgen/Structure.cpp
	src/sim/levelgen/LevelGenerator.cpp
	src/sim/levelgen/LevelGenerator.hpp

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

if (USE_ASAN)
	message("Enabling ASAN")

	target_compile_options(${executable} PRIVATE -fsanitize=address)
	target_link_options(${executable} PRIVATE -fsanitize=address)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${executable} PRIVATE -fsanitize=undefined)
		target_link_options(${executable} PRIVATE -fsanitize=undefined)
	endif()
endif()
