cmake_minimum_required(VERSION 3.0)
project(Darktimes)
add_subdirectory(win)
set(CMAKE_CXX_STANDARD 17)

set(executable Darktimes)

set(sources
	src/main.cpp

	src/Darktimes.hpp

	# top level nonsense
	src/Renderable.hpp
	src/TextureDefinitions.hpp
	src/TextureDefinitions.cpp
	src/Textures.txt
	src/SyncObjectManager.hpp
	src/LevelRenderState.hpp
	src/RenderState.hpp
	src/Input.hpp
	src/RenderState.hpp
	src/RandomNumberGenerator.cpp
	src/RandomNumberGenerator.hpp

	# render
	src/render/Renderer.hpp
	src/render/Renderer.cpp
	src/render/RendererBackend.hpp

	# gl renderer
	src/render/gl/GLRendererBackend.hpp
	src/render/gl/GLRendererBackend.cpp
	src/render/gl/FloorTextureCollection.hpp
	src/render/gl/FloorTextureCollection.cpp
	src/render/gl/AtlasTextureCollection.hpp
	src/render/gl/AtlasTextureCollection.cpp
	src/render/gl/GLSubRenderer.hpp
	src/render/gl/GLStaticFloorRenderer.hpp
	src/render/gl/GLStaticFloorRenderer.cpp
	src/render/gl/GLStaticAtlasRenderer.hpp
	src/render/gl/GLStaticAtlasRenderer.cpp
	src/render/gl/GLDynamicAtlasRenderer.hpp
	src/render/gl/GLDynamicAtlasRenderer.cpp

	# sim
	src/sim/World.hpp
	src/sim/Simulation.cpp
	src/sim/Simulation.hpp

	# entities
	src/sim/entity/Entity.hpp
	src/sim/entity/PlayerEntity.hpp
	src/sim/entity/PlayerEntity.cpp

	# components
	src/sim/component/Component.hpp
	src/sim/component/PhysicalComponent.hpp
	src/sim/component/RenderableComponent.hpp
	src/sim/component/PlayerComponent.hpp

	# systems
	src/sim/system/PlayerSystem.hpp
	src/sim/system/PlayerSystem.cpp

	# level gen nonsense
	src/sim/levelgen/LevelObjects.hpp
	src/sim/levelgen/PropDefs.hpp
	src/sim/levelgen/StructureDefs.hpp
	src/sim/levelgen/Structure.hpp
	src/sim/levelgen/Structure.cpp
	src/sim/levelgen/LevelGenerator.hpp
	src/sim/levelgen/LevelGenerator.cpp
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
