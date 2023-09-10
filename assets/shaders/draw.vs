#version 330 core

uniform mat4 projection;
uniform mat4 view;

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in float v_size;

out vec4 m_color;

void main(void) {
  m_color = v_color;
  gl_Position = projection * view * vec4(v_position, 1);
  float x = max(v_size, 1);
  gl_PointSize = min((x * 200) / gl_Position.z, x);
}