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
    include/world.h src/world.cpp
    include/glad/gl.h src/gl.c
    include/KHR/khrplatform.h
    include/stb/stb_image.h src/stb/stb.cpp
    include/tarragon-imconfig.h
    src/main.cpp
)

set(imgui_FILES
    ${imgui_SOURCE_DIR}/imgui.h ${imgui_SOURCE_DIR}/imgui_internal.h ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_FILES} ${imgui_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${${TARGET_NAME}_FILES})
cmake_path(GET imgui_SOURCE_DIR PARENT_PATH imgui_PARENT_DIR)
source_group(TREE ${imgui_PARENT_DIR} FILES ${imgui_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(${TARGET_NAME} PRIVATE include/ ${imgui_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC
    libtg
    glfw
    glm::glm
)
target_compile_definitions(${TARGET_NAME} PRIVATE IMGUI_USER_CONFIG="tarragon-imconfig.h")


set(TEXTURES
    "res/rock-diffuse.png" ;
)
set(RESOURCES_TEXTURES)

set(SHADERS
    "shaders/chunk.vert" ;
    "shaders/chunk.frag" ;
    "shaders/chunk_normals.vert" ;
    "shaders/chunk_normals.frag" ;
)
set(RESOURCES_SHADERS)

foreach(TEXTURE ${TEXTURES})
    get_filename_component(FILE_NAME ${TEXTURE} NAME)
    set(TEXTURE_OUT "${CMAKE_CURRENT_BINARY_DIR}/${TEXTURE}")
    add_custom_command(
        OUTPUT ${TEXTURE_OUT}
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE}" ${TEXTURE_OUT}
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE}")
    list(APPEND RESOURCES_TEXTURES ${TEXTURE_OUT})
endforeach(TEXTURE)

foreach(SHADER_SOURCE ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER_SOURCE} NAME)
    set(SHADER_OUT "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_SOURCE}")
    add_custom_command(
        OUTPUT ${SHADER_OUT}
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SOURCE}" ${SHADER_OUT}
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SOURCE}")
    list(APPEND RESOURCES_SHADERS ${SHADER_SOURCE})
endforeach(SHADER_SOURCE)

add_custom_target(
    CopyShaders 
    DEPENDS ${RESOURCES_SHADERS}
)

add_custom_target(
    CopyTextures
    DEPENDS ${RESOURCES_TEXTURES}
)

add_dependencies(${TARGET_NAME} CopyShaders)
add_dependencies(${TARGET_NAME} CopyTextures)
