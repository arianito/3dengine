#version 330 core

out vec4 FragColor;

in vec4 FragPosition;
in vec3 Normal;
in vec4 ShadowCoord;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform sampler2D shadowMap;

void main()
{
    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPosition.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = specularStrength * spec * lightColor;


    float shadow = 0;
    vec3 coords = ShadowCoord.xyz / ShadowCoord.w;

    if (coords.z <= 1) {
        coords = (coords + 1.0f) / 2.0f;



        float currentDepth = coords.z;
        int samples = 1;
        vec2 pix = 1.0f / textureSize(shadowMap, 0);
        float bias = max((1.0f - dot(lightDir, norm)) * length(pix) * 2.0f, 0.005f);
        for(int i = -samples; i <= samples; i++) {
            for(int j = -samples; j <= samples; j++) {

                float closestDepth = texture(shadowMap, coords.xy + vec2(i, j) * pix).r;
                if (currentDepth > closestDepth + bias) shadow += 1.0f;

            }
        }
        shadow /= pow((samples * 2 + 1.0), 2);
    }


    float fresnel = pow(1 - clamp(dot(norm, viewDir), 0, 1), 2) * dot(viewDir, norm) * 0.5f;

    vec3 result = (ambient + fresnel + (diffuse + specular) * (1 - shadow)) * objectColor;

    FragColor = vec4(result, 1.0);
}