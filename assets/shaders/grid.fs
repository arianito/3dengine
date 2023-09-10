 #version 330 core 

uniform float alpha;

in vec4 m_color;
in float m_depth;
in vec3 fragPos;
out vec4 FragColor;

void main(void) {
    FragColor = m_color;
    FragColor.a = alpha * min(m_color.a * 1000 / (m_depth * m_depth), 0.2);
    FragColor.a *= min(5000.0f / sqrt((fragPos.x * fragPos.x) + (fragPos.y * fragPos.y) + (fragPos.z * fragPos.z)), 0.5);
}