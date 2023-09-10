#version 330 core 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec4 v_color;

out vec4 m_color;
out float m_depth;
out vec3 fragPos;

void main(void) {
    gl_Position = projection * view * world * vec4(v_position, 1);
    
    fragPos = vec3(world * vec4(v_position, 1.0));

    m_color = v_color;
    m_depth = 0.1f * gl_Position.z;
}
