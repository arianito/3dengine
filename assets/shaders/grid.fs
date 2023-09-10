 #version 330 core 

uniform float alpha;

in vec4 m_color;
in float m_depth;
out vec4 FragColor;

void main(void) {
    FragColor = m_color;
    FragColor.a = alpha * (0.45 - min(m_color.a * pow(m_depth * 0.1f, 1), 0.45));
}