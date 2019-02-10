cmake_minimum_required(VERSION 3.0)

project(darktimes)
add_subdirectory(win)

add_executable( darktimes
	src/main.cpp
	src/darktimes.h
	src/world.cpp
	src/world.h
	src/render/renderer.cpp
	src/render/renderer.h
	src/entity/entity.cpp
	src/entity/entity.h
)

target_link_libraries(darktimes PRIVATE win)
target_include_directories(darktimes PRIVATE win/include)
