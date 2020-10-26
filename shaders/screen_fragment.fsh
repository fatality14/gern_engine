#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

uniform vec3 cameraPos;

uniform sampler2D textureColorBuffer0;

uniform vec3 lightPos0;

out vec4 fs_color;

struct Material{
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;

    float specularHighlights;
    float opticalDensity;
    float dissolve;
};

uniform Material material;

void main()
{
    vec4 tex = texture(textureColorBuffer0, vs_texcoord);
//    tex.r = 1 - tex.r;
//    tex.g = 1 - tex.g;
//    tex.b = 1 - tex.b;

    fs_color = tex;
}
