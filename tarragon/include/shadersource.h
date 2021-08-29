#pragma once

#include <string_view>

namespace tarragon
{
    static constexpr std::string_view VERTEX_SOURCE =
        R"xx(
#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vert_color;
out vec3 vert_normal;

void main()
{
    vert_color = color;
    vert_normal = normal;
    gl_Position = (projection * view * model) * vec4(pos, 1.0);
}

)xx";

    static constexpr std::string_view FRAGMENT_SOURCE =
        R"xx(
#version 460

in vec4 vert_color;
in vec3 vert_normal;

out vec4 frag_color;

void main()
{
    frag_color = mix(vert_color, vec4(vert_normal, 1.0), 0.7);
}

)xx";
}
