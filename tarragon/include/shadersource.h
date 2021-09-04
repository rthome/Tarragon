#pragma once

#include <string_view>

namespace tarragon
{
    static constexpr std::string_view NORMAL_VS_SOURCE =
        R"xx(
#version 460

layout (location = 0) in vec3 pos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}
)xx";

    static constexpr std::string_view NORMAL_FS_SOURCE =
        R"xx(
#version 460

out vec4 frag_color;

void main()
{
    frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}
)xx";




    static constexpr std::string_view VERTEX_SOURCE =
        R"xx(
#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 vert_pos;
out vec3 vert_normal;
out vec2 tex_coords;

void main()
{
    vert_pos = vec3(Model * vec4(pos, 1.0));
    vert_normal = normal;
    tex_coords = texcoord;

    gl_Position = Projection * View * vec4(vert_pos, 1.0);
}

)xx";

    static constexpr std::string_view FRAGMENT_SOURCE =
        R"xx(
#version 460

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

in vec3 vert_pos;
in vec3 vert_normal;
in vec2 tex_coords;

uniform Light L;
uniform sampler2D TexDiffuse;

out vec4 frag_color;

void main()
{
    vec4 vert_color = texture(TexDiffuse, tex_coords);

    vec3 ambient = L.ambient * vert_color.rgb;

    vec3 norm = normalize(vert_normal);
    vec3 light_dir = normalize(L.position - vert_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = L.diffuse * diff * vert_color.rgb;

    float distance    = length(L.position - vert_pos);
    float attenuation = 1.0 / (L.constant + L.linear * distance + L.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;

    vec3 result = ambient + diffuse;
    frag_color = vec4(result, 1.0);
}

)xx";
}
