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
