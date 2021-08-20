cmake_minimum_required(VERSION 3.20)

project(tarragon VERSION 0.1 LANGUAGES CXX C)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(libtg)
add_subdirectory(tarragon)
add_subdirectory(tarragon-test)

enable_testing()
