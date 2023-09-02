 #version 330 core 

in vec4 m_color;
in float m_depth;
out vec4 FragColor;

void main(void) {
    FragColor = m_color;
    FragColor.a = min(m_color.a * 1000 / (m_depth * m_depth), 0.2);
}