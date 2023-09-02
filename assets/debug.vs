#version 330 core

uniform mat4 projection;

layout(location = 0) in vec3 v_position;

out vec4 m_color;

void main(void) {

  m_color = vec4(1, 1, 1, 1);
  gl_Position = projection * vec4(v_position, 1);
}