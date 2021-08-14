cmake_minimum_required(VERSION 3.20)

add_library(libtg)
target_sources(libtg PRIVATE
    include/synchronized.h
    include/noise/generator.h noise/generator.cpp
    include/noise/modules.h noise/modules.cpp
)

set_target_properties(libtg PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    DEBUG_POSTFIX "d"
)
target_include_directories(libtg PUBLIC include/)
target_link_libraries(libtg PUBLIC
    glm::glm
)
