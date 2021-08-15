cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME tarragon)
set(${TARGET_NAME}_FILES
    include/glad/gl.h
    include/framelimit.h
    include/KHR/khrplatform.h
    src/main.cpp
    src/gl.c
)

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${${TARGET_NAME}_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(${TARGET_NAME} PRIVATE include/)
target_link_libraries(${TARGET_NAME} PUBLIC
    libtg
    glfw
    glm::glm
)
