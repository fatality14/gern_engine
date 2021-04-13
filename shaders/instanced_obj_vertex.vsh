#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in mat4 model;

out vec3 vs_position;
out vec2 vs_texcoord;
out vec3 vs_normal;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vs_position = vec4(model * vec4(vertex_position, 1.f)).xyz;
    vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
    vs_normal = mat3(model) * vertex_normal;

    gl_Position = projectionMatrix * viewMatrix * model * vec4(vertex_position, 1.f);
}
