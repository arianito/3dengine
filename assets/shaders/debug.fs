#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform int character;

void main() {
  float p = 1 / 16.0;
  vec2 coords = TexCoord * p;

  coords.x += p * (character % 16);
  coords.y += p * (character / 16);

  vec4 tex = texture(texture1, coords);
  FragColor = vec4(1, 1, 1, tex.r);
}