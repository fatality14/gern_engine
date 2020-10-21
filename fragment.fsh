#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform sampler2D texture0;
uniform sampler2D specularTex;

uniform vec3 lightPos0;
uniform vec3 cameraPos;

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
    vec3 posToLightDirVec = normalize(lightPos0 - vs_position);
    float diffuse = dot(posToLightDirVec, normalize(vs_normal));
    vec3 diffuseFinal = material.diffuseColor * diffuse;

    vec3 lightToPosDirVec = normalize(vs_position - lightPos0);
    vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
    vec3 posToViewDirVec = normalize(cameraPos - vs_position);
    float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), 30);
    vec3 specularFinal = material.specularColor * specularConstant * texture(specularTex, vs_texcoord).rgb;

    vec4 tex = texture(texture0, vs_texcoord);
    vec3 lightning = (material.ambientColor + diffuseFinal + specularFinal) * 1/length(lightPos0 - vs_position) * 5 * vec3(0.788f, 0.584f, 0.725f);

    fs_color = tex * vec4(lightning, 1.f);
}
