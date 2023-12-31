#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec4 FragPosition;
out vec3 Normal;
out vec4 ShadowCoord;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 viewProjection;
uniform mat4 depthViewProjection;

void main()
{
    TexCoord = aTexCoord;

    FragPosition = model * vec4(aPosition, 1.0);

    Normal = mat3(transpose(inverse(model))) * aNormal;

    ShadowCoord = depthViewProjection * FragPosition;

    gl_Position = viewProjection * FragPosition;
}