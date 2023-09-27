#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out vec4 FragPosition;
out vec3 Normal;
out vec4 ShadowCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 depthBias;

void main()
{
    FragPosition = model * vec4(aPosition, 1.0);

    Normal = mat3(transpose(inverse(model))) * aNormal;

    ShadowCoord = depthBias * vec4(aPosition, 1.0);

    gl_Position = projection * view * FragPosition;
}