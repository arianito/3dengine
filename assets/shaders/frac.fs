#version 330 core
#define PI 3.14159265359

out vec4 FragColor;

in vec4 FragPosition;
in vec3 Normal;
in vec2 TexCoord;

uniform float time;
uniform vec2 mouse;

float circle(vec2 pos, float rad, float dm) {
    vec2 origin = TexCoord - vec2(0.5, 0.5);
    float r = length(pos - origin);
    float f = rad;
    return 1 - smoothstep(f, f + dm, r);
}

int noise_1d(int px, int seed) {
    const int SQ5_Bit_Noise1 = 0xd2a80a3f;
    const int SQ5_Bit_Noise2 = 0xa884f197;
    const int SQ5_Bit_Noise3 = 0x6C736F4B;
    const int SQ5_Bit_Noise4 = 0xB79F3ABB;
    const int SQ5_Bit_Noise5 = 0x1b56c4f5;
    int MangledBits = px;
    MangledBits *= SQ5_Bit_Noise1;
    MangledBits += seed;
    MangledBits ^= (MangledBits >> 9);
    MangledBits += SQ5_Bit_Noise2;
    MangledBits ^= (MangledBits >> 11);
    MangledBits *= SQ5_Bit_Noise3;
    MangledBits ^= (MangledBits >> 13);
    MangledBits += SQ5_Bit_Noise4;
    MangledBits ^= (MangledBits >> 15);
    MangledBits *= SQ5_Bit_Noise5;
    MangledBits ^= (MangledBits >> 17);
    return MangledBits;
}

int noise_2d(int px, int py, int seed) {
    const int Large_Prime = 198491317;
    return noise_1d(px + (Large_Prime * py), seed);
}
float noise(int px, int py, int seed) {
    const float One_Over_Max_Uint = 1.0 / 0x7FFFFFFF;
    return One_Over_Max_Uint * noise_2d(px, py, seed);
}

float grad(int x, int y, float x0, float y0, int seed) {
    float t = noise(x, y, seed) * PI * 2;
    return ((x0 - x) * cos(t)) + ((y0 - y) * sin(t));
}

float fade(float t) {
    return ((6 * t - 15) * t + 10) * t * t * t;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
float perlin(vec2 pos, float scale, int seed) {
    float ScaleClamped = scale <= 0 ? 1.0f : scale;
    float Xs = pos.x / ScaleClamped;
    float Ys = pos.y / ScaleClamped;
    int X0 = int(floor(Xs));
    int X1 = X0 + 1;
    int Y0 = int(floor(Ys));
    int Y1 = Y0 + 1;
    float Dx = fade(Xs - X0);
    float Dy = fade(Ys - Y0);

    return lerp(
        lerp(
            grad(X0, Y0, Xs, Ys, seed),
            grad(X1, Y0, Xs, Ys, seed), Dx),
        lerp(
            grad(X0, Y1, Xs, Ys, seed),
            grad(X1, Y1, Xs, Ys, seed), Dx), Dy);
}


void main()
{
    vec2 origin = TexCoord - vec2(0.5, 0.5);

    float r = length(origin) * 10.0f;
    float angle = atan(origin.y, origin.x);

    float t = time * 2.0    ;
    vec4 color = vec4(0, 0, 0, 0);
    float val = 0;

    val = perlin(vec2(r, cos(angle)) + t- vec2(t * 5), 0.5f, 1) * 2;

    color += vec4(0.8f, 0.3f, 0.2f, 1.0f) * val;

    val = perlin(vec2(cos(angle), r) + t, 1.2f, 31) * 2;

    color += vec4(0.2f, 0.4f, 0.8f, 1.0f) * val;


    color *= circle(vec2(0, 0), 0.45, 0.1);



    FragColor = color;
}
