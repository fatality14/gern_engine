#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;

out vec3 vs_position;
out vec3 vs_texcoord;
out vec3 vs_normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vs_position = vec4(modelMatrix * vec4(vertex_position, 1.f)).xyz;
    vs_texcoord = vertex_position;
    vs_normal = mat3(modelMatrix) * vertex_normal;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex_position, 1.f);
}
