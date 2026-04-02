cmake_minimum_required(VERSION 3.10)
project(Darktimes)
add_subdirectory(win)
set(CMAKE_CXX_STANDARD 17)

set(executable Darktimes)

set(sources
	src/main.cpp

	# top level nonsense
	src/Renderable.hpp
	src/Texture.hpp
	src/RandomNumberGenerator.hpp
	src/RandomNumberGenerator.cpp
	src/GameInput.hpp

	# render
	src/render/Renderer.hpp
	src/render/Renderer.cpp
	src/render/RendererBackend.hpp
	src/render/TextureAssetMap.hpp
	src/render/TextureAssetMap.txt

	# gl renderer
	src/render/gl/GLRendererBackend.hpp
	src/render/gl/GLRendererBackend.cpp
	src/render/gl/GLRendererBase.hpp
	src/render/gl/GLConstants.hpp
	src/render/gl/GLFloorRenderer.hpp
	src/render/gl/GLFloorRenderer.cpp
	src/render/gl/GLAtlasRenderer.hpp
	src/render/gl/GLAtlasRenderer.cpp

	# sim
	src/sim/Simulation.hpp
	src/sim/Simulation.cpp
	src/sim/Game.hpp
	src/sim/Game.cpp
	src/sim/World.hpp

	# level gen nonsense
	src/sim/levelgen/LevelObjects.hpp
	src/sim/levelgen/LevelObjectsInternal.hpp
	src/sim/levelgen/PropDefs.hpp
	src/sim/levelgen/StructureDefs.hpp
	src/sim/levelgen/Structure.hpp
	src/sim/levelgen/LevelGenerator.hpp
	src/sim/levelgen/LevelGenerator.cpp

	src/sim/entity/Entities.hpp
	src/sim/component/Components.hpp
	src/sim/system/Systems.hpp

	src/sim/entity/Entity.cpp
	src/sim/entity/PlayerEntity.cpp
	src/sim/system/PlayerSystem.cpp
)

if (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
	add_executable(${executable} WIN32 ${sources})
else ()
	add_executable(${executable} ${sources})
endif ()

target_link_libraries(${executable} win)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${executable} PRIVATE -fno-exceptions)
elseif (WIN32)
	# /w3 is too aggressive
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif ()
endif ()

if (USE_ASAN)
	message("Enabling ASAN")

	target_compile_options(${executable} PRIVATE -fsanitize=address)
	target_link_options(${executable} PRIVATE -fsanitize=address)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${executable} PRIVATE -fsanitize=undefined)
		target_link_options(${executable} PRIVATE -fsanitize=undefined)
	endif ()
endif ()
