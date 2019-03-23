cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)

project(darktimes)
add_subdirectory(win)

add_executable( darktimes
	src/main.cpp
	src/darktimes.h
	src/world.cpp
	src/world.h
	src/level.cpp
	src/level.h

	src/render/renderer.cpp
	src/render/renderer.h
	src/render/quadpass.cpp
	src/render/quadpass.vert
	src/render/quadpass.frag

	src/entity/entity.cpp
	src/entity/entity.h
	src/entity/wall.cpp

	build/assets
)

if(WIN32)
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif()
endif()

file(GLOB assetfiles ${CMAKE_CURRENT_LIST_DIR}/asset/*)
list(APPEND assetfiles ${CMAKE_CURRENT_LIST_DIR}/src/render/quadpass.vert ${CMAKE_CURRENT_LIST_DIR}/src/render/quadpass.frag)

add_custom_command(
	OUTPUT
		${CMAKE_CURRENT_LIST_DIR}/build/assets
	COMMAND
		cd ${CMAKE_CURRENT_LIST_DIR}/asset && atlas main1.atlas main1.adesc && roll ../build/assets
	DEPENDS
		${assetfiles}
)

target_link_libraries(darktimes PRIVATE win)
target_include_directories(darktimes PRIVATE win/include)
