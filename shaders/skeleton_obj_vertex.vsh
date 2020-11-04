#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in ivec3 vertex_joints;
layout (location = 4) in vec3 vertex_weights;

const int JOINTAMOUNT = 50;//make this one uniform

out vec3 vs_position;
out vec2 vs_texcoord;
out vec3 vs_normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 jointTransforms[JOINTAMOUNT];

void main()
{
    vec4 newVertexPos = vec4(0.0);
    vec4 newVertexNorm = vec4(0.0);
    for(int i = 0; i < 3; i++){
        vec4 jointPosShift = jointTransforms[vertex_joints[i]] * vec4(vertex_position, 1.0);
        newVertexPos += jointPosShift * vertex_weights[i];

        vec4 jointNormShift = jointTransforms[vertex_joints[i]] * vec4(vertex_normal, 1.0);
        newVertexNorm += jointNormShift * vertex_weights[i];
    }

    vs_position = vec4(modelMatrix * newVertexPos).xyz;
    vs_normal = mat3(modelMatrix) * newVertexNorm.xyz;
    vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * newVertexPos;
//    vs_position = vec4(modelMatrix * vec4(vertex_position, 1.f)).xyz;
//    vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
//    vs_normal = mat3(modelMatrix) * vertex_normal;

//    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex_position, 1.f);
}
