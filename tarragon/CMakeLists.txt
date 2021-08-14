cmake_minimum_required(VERSION 3.20)

add_executable(tarragon)
target_sources(tarragon PRIVATE
    include/glad/gl.h
    include/framelimit.h
    include/KHR/khrplatform.h
    src/main.cpp
    src/gl.c
)

set_target_properties(tarragon PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(tarragon PRIVATE include/)
target_link_libraries(tarragon PUBLIC
    libtg
    glfw
    glm::glm
)
