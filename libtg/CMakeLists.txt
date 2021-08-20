cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME libtg)
set(${TARGET_NAME}_FILES
    include/common.h
    include/signal.h
    include/synchronized.h
    include/noise/chunk.h
    include/noise/generator.h src/noise/generator.cpp
    include/noise/modules.h src/noise/modules.cpp
)

add_library(${TARGET_NAME} STATIC)
target_sources(libtg PRIVATE ${${TARGET_NAME}_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${${TARGET_NAME}_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    DEBUG_POSTFIX "d"
)
target_include_directories(${TARGET_NAME} PUBLIC include/)
target_link_libraries(${TARGET_NAME} PUBLIC
    glm::glm
)
