 #version 330 core 

uniform float alpha;

in vec4 m_color;
in float m_depth;
out vec4 FragColor;

void main(void) {
    FragColor = m_color;
    FragColor.a = alpha * min(m_color.a * 2000 / (m_depth * m_depth), 0.2);
}