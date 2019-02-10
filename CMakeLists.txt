cmake_minimum_required(VERSION 3.0)

project(darktimes)
add_subdirectory(win)

add_executable( darktimes
	src/main.cpp
)

target_link_libraries(darktimes PRIVATE win)
target_include_directories(darktimes PRIVATE win/include)
