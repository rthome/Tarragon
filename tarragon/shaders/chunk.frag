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
