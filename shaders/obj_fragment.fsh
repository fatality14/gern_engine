#version 440

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

uniform vec3 cameraPos;

uniform sampler2D texture0;
uniform sampler2D specularTex;

uniform samplerCube skybox;

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

vec3 testFoo(in vec3 arg);

vec3 calcDiffuseLightVec(in vec3 lightPosition, in vec3 color){
    //vec from pos to light
    vec3 posToLightDirVec = normalize(lightPosition - vs_position);
    //cos between normal and from light to pos vec
    float diffuseAngle = dot(posToLightDirVec, normalize(vs_normal));
    //vec of three coefs
    return 0.3f * color * diffuseAngle;
}
vec3 calcSpecularLightVec(in vec3 lightPosition, in vec3 color, in float specularHighlights, in sampler2D tex){
    //vec from light to pos
    vec3 lightToPosDirVec = normalize(vs_position - lightPosition);
    //reflected from fragment light to pos vec
    vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
    //vec from pos to camera
    vec3 posToViewDirVec = normalize(cameraPos - vs_position);
    //dot calcs angle between posToViewDirVec and reflectDirVec
    //if cos < 0, then value is 0
    //the higher specularHighlights, the more color tight
    float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), specularHighlights);
    //calculates final
    return color * specularConstant * texture(tex, vs_texcoord).rgb;
}
float calcAttenuation(in vec3 lightPosition, in float power){
    float d = length(lightPosition - vs_position);
    return clamp(power/d, 0.0, 1.0);
}
vec3 calcFullLight(in vec3 lightPosition, Material material, in float attenuationPower, in sampler2D specularTexture){
    vec3 diffuseLight = calcDiffuseLightVec(lightPosition, material.diffuseColor);
    vec3 specularLight = calcSpecularLightVec(lightPosition, material.specularColor, material.specularHighlights, specularTexture);
    float attenuation = calcAttenuation(lightPosition, attenuationPower);

    return attenuation * (material.ambientColor + diffuseLight + specularLight);
}

void main()
{
    vec3 lightning = testFoo(calcFullLight(lightPos0, material, 50, specularTex));
    vec4 tex = texture(texture0, vs_texcoord);

    //vec3 posToCamVec = normalize(vs_position - cameraPos);

//    vec3 reflection = reflect(posToCamVec, normalize(vs_normal));
//    vec4 tex = texture(skybox, reflection);

//    vec3 ref = refract(posToCamVec, vs_normal, 1/1.2);
//    vec4 tex = texture(skybox, ref);

    fs_color = tex * vec4(lightning, 1.f);
}
