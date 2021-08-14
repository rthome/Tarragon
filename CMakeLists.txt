cmake_minimum_required(VERSION 3.20)

project(tarragon VERSION 0.1 LANGUAGES CXX C)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

add_subdirectory(libtg)
add_subdirectory(tarragon)
add_subdirectory(tarragon-test)

enable_testing()
