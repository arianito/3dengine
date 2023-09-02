#version 330 core 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform float alpha;

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec4 v_color;

out vec4 m_color;
out float m_depth;

void main(void) {
    gl_Position = projection * view * world * vec4(v_position, 1);
    m_color = v_color;
    m_depth = 0.1 * gl_Position.z / alpha;
}
