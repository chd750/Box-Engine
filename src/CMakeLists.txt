cmake_minimum_required(VERSION 3.23)
project(BoxEngine)

add_subdirectory(SDL3_git EXCLUDE_FROM_ALL)
add_executable(BoxEngine)
target_sources(BoxEngine PRIVATE main.c)
target_link_libraries(BoxEngine PRIVATE SDL3::SDL3)
