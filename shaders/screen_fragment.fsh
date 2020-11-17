#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

uniform vec3 cameraPos;

uniform sampler2D textureColorBuffer0;
uniform sampler2D textureColorBuffer1;

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

const float offset = 1.0 / 300.0;

void main()
{
    vec4 tex = texture(textureColorBuffer1, vs_texcoord);
    fs_color = tex;

//    vec2 offsets[9] = vec2[](
//        vec2(-offset,  offset), // top-left
//        vec2( 0.0f,    offset), // top-center
//        vec2( offset,  offset), // top-right
//        vec2(-offset,  0.0f),   // center-left
//        vec2( 0.0f,    0.0f),   // center-center
//        vec2( offset,  0.0f),   // center-right
//        vec2(-offset, -offset), // bottom-left
//        vec2( 0.0f,   -offset), // bottom-center
//        vec2( offset, -offset)  // bottom-right
//    );
//    float kernel[9] = float[](
//        1.0 / 16, 2.0 / 16, 1.0 / 16,
//        2.0 / 16, 4.0 / 16, 2.0 / 16,
//        1.0 / 16, 2.0 / 16, 1.0 / 16
//    );

//    vec3 sampleTex[9];
//    for(int i = 0; i < 9; i++)
//    {
//        sampleTex[i] = vec3(texture(textureColorBuffer0, vs_texcoord.st + offsets[i]));
//    }
//    vec3 col = vec3(0.0);
//    for(int i = 0; i < 9; i++)
//        col += sampleTex[i] * kernel[i];

//    fs_color = vec4(col, 1.0);
}
