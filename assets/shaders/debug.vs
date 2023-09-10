#version 330 core

uniform mat4 projection;

layout(location = 0) in vec3 v_position;

void main(void) {

  gl_Position = projection * vec4(v_position, 1);
}