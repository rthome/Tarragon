cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME tarragon)
set(${TARGET_NAME}_FILES
    include/camera.h src/camera.cpp
    include/chunk.h src/chunk.cpp
    include/chunkcache.h src/chunkcache.cpp
    include/chunkrenderer.h src/chunkrenderer.cpp
    include/chunktransfer.h src/chunktransfer.cpp
    include/chunkupdater.h src/chunkupdater.cpp
    include/clock.h
    include/component.h
    include/engine.h src/engine.cpp
    include/framelimit.h
    include/input.h src/input.cpp
    include/shader.h
    include/shadersource.h
    include/glad/gl.h src/gl.c
    include/KHR/khrplatform.h
    include/stb/stb_image.h src/stb/stb.cpp
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


set(TEXTURES
    "res/rock-diffuse.png" ;
)
set(RESOURCES_TEXTURES)

foreach(TEXTURE ${TEXTURES})
    get_filename_component(FILE_NAME ${TEXTURE} NAME)
    set(TEXTURE_OUT "${CMAKE_CURRENT_BINARY_DIR}/res/${FILE_NAME}")
    add_custom_command(
        OUTPUT ${TEXTURE_OUT}
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE}" ${TEXTURE_OUT}
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE}"
    )
    list(APPEND RESOURCES_TEXTURES ${TEXTURE_OUT})
endforeach(TEXTURE)

add_custom_target(
    CopyTextures
    DEPENDS ${RESOURCES_TEXTURES}
)

add_dependencies(${TARGET_NAME} CopyTextures)