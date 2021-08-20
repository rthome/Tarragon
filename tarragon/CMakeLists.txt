cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME tarragon)
set(${TARGET_NAME}_FILES
    include/clock.h
    include/component.h
    include/framelimit.h
    include/shader.h
    include/KHR/khrplatform.h
    include/camera.h src/camera.cpp
    include/input.h src/input.cpp
    include/glad/gl.h src/gl.c
    src/main.cpp
)

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${${TARGET_NAME}_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(${TARGET_NAME} PRIVATE include/)
target_link_libraries(${TARGET_NAME} PUBLIC
    libtg
    glfw
    glm::glm
)
