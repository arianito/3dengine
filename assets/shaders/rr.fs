#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D shadowMap;

void main()
{
    vec4 p = texture(shadowMap, TexCoord);

    FragColor = vec4(vec3(p), 1);
}