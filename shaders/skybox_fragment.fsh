#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec3 vs_texcoord;
in vec3 vs_normal;

uniform samplerCube skybox;

out vec4 fs_color;

void main()
{
    fs_color = texture(skybox, vs_texcoord);
}
