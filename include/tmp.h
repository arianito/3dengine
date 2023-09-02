
/*
inline Mat4 mat4_add(Mat4 a, Mat4 b)
{
    return (Mat4){
        .a11 = a.a11 + b.a11,
        .a12 = a.a12 + b.a12,
        .a13 = a.a13 + b.a13,
        .a14 = a.a14 + b.a14,
        .a21 = a.a21 + b.a21,
        .a22 = a.a22 + b.a22,
        .a23 = a.a23 + b.a23,
        .a24 = a.a24 + b.a24,
        .a31 = a.a31 + b.a31,
        .a32 = a.a32 + b.a32,
        .a33 = a.a33 + b.a33,
        .a34 = a.a34 + b.a34,
        .a41 = a.a41 + b.a41,
        .a42 = a.a42 + b.a42,
        .a43 = a.a43 + b.a43,
        .a44 = a.a44 + b.a44};
}
inline Mat4 mat4_sub(Mat4 a, Mat4 b)
{
    return (Mat4){
        .a11 = a.a11 - b.a11,
        .a12 = a.a12 - b.a12,
        .a13 = a.a13 - b.a13,
        .a14 = a.a14 - b.a14,
        .a21 = a.a21 - b.a21,
        .a22 = a.a22 - b.a22,
        .a23 = a.a23 - b.a23,
        .a24 = a.a24 - b.a24,
        .a31 = a.a31 - b.a31,
        .a32 = a.a32 - b.a32,
        .a33 = a.a33 - b.a33,
        .a34 = a.a34 - b.a34,
        .a41 = a.a41 - b.a41,
        .a42 = a.a42 - b.a42,
        .a43 = a.a43 - b.a43,
        .a44 = a.a44 - b.a44};
}

inline float mat4_det(Mat4 a)
{
    return a.a11 * a.a22 * a.a33 * a.a44 -
           a.a11 * a.a22 * a.a34 * a.a43 -
           a.a11 * a.a23 * a.a32 * a.a44 +
           a.a11 * a.a23 * a.a34 * a.a42 +
           a.a11 * a.a24 * a.a32 * a.a43 -
           a.a11 * a.a24 * a.a33 * a.a42 -
           a.a12 * a.a21 * a.a33 * a.a44 +
           a.a12 * a.a21 * a.a34 * a.a43 +
           a.a12 * a.a23 * a.a31 * a.a44 -
           a.a12 * a.a23 * a.a34 * a.a41 -
           a.a12 * a.a24 * a.a31 * a.a43 +
           a.a12 * a.a24 * a.a33 * a.a41 +
           a.a13 * a.a21 * a.a32 * a.a44 -
           a.a13 * a.a21 * a.a34 * a.a42 -
           a.a13 * a.a22 * a.a31 * a.a44 +
           a.a13 * a.a22 * a.a34 * a.a41 +
           a.a13 * a.a24 * a.a31 * a.a42 -
           a.a13 * a.a24 * a.a32 * a.a41 -
           a.a14 * a.a21 * a.a32 * a.a43 +
           a.a14 * a.a21 * a.a33 * a.a42 +
           a.a14 * a.a22 * a.a31 * a.a43 -
           a.a14 * a.a22 * a.a33 * a.a41 -
           a.a14 * a.a23 * a.a31 * a.a42 +
           a.a14 * a.a23 * a.a32 * a.a41;
}
inline Mat4 mat4_invert(Mat4 a)
{
    float det = mat4_det(a);
    if (det == 0)
        return mat4_identity;
    return (Mat4){
        .a11 = (a.a22 * a.a33 * a.a44 + a.a23 * a.a34 * a.a42 +
                a.a24 * a.a32 * a.a43 - a.a22 * a.a34 * a.a43 -
                a.a23 * a.a32 * a.a44 - a.a24 * a.a33 * a.a42) /
               det,

        .a12 = (a.a12 * a.a34 * a.a43 + a.a13 * a.a32 * a.a44 +
                a.a14 * a.a33 * a.a42 - a.a12 * a.a33 * a.a44 -
                a.a13 * a.a34 * a.a42 - a.a14 * a.a32 * a.a43) /
               det,

        .a13 = (a.a12 * a.a23 * a.a44 + a.a13 * a.a24 * a.a42 +
                a.a14 * a.a22 * a.a43 - a.a12 * a.a24 * a.a43 -
                a.a13 * a.a22 * a.a44 - a.a14 * a.a23 * a.a42) /
               det,

        .a14 = (a.a12 * a.a24 * a.a33 + a.a13 * a.a22 * a.a34 +
                a.a14 * a.a23 * a.a32 - a.a12 * a.a23 * a.a34 -
                a.a13 * a.a24 * a.a32 - a.a14 * a.a22 * a.a33) /
               det,

        .a21 = (a.a21 * a.a34 * a.a43 + a.a23 * a.a31 * a.a44 +
                a.a24 * a.a33 * a.a41 - a.a21 * a.a33 * a.a44 -
                a.a23 * a.a34 * a.a41 - a.a24 * a.a31 * a.a43) /
               det,

        .a22 = (a.a11 * a.a33 * a.a44 + a.a13 * a.a34 * a.a41 +
                a.a14 * a.a31 * a.a43 - a.a11 * a.a34 * a.a43 -
                a.a13 * a.a31 * a.a44 - a.a14 * a.a33 * a.a41) /
               det,

        .a23 = (a.a11 * a.a24 * a.a43 + a.a13 * a.a21 * a.a44 +
                a.a14 * a.a23 * a.a41 - a.a11 * a.a23 * a.a44 -
                a.a13 * a.a24 * a.a41 - a.a14 * a.a21 * a.a43) /
               det,

        .a24 = (a.a11 * a.a23 * a.a34 + a.a13 * a.a24 * a.a31 +
                a.a14 * a.a21 * a.a33 - a.a11 * a.a24 * a.a33 -
                a.a13 * a.a21 * a.a34 - a.a14 * a.a23 * a.a31) /
               det,

        .a31 = (a.a21 * a.a32 * a.a44 + a.a22 * a.a34 * a.a41 +
                a.a24 * a.a31 * a.a42 - a.a21 * a.a34 * a.a42 -
                a.a22 * a.a31 * a.a44 - a.a24 * a.a32 * a.a41) /
               det,

        .a32 = (a.a11 * a.a34 * a.a42 + a.a12 * a.a31 * a.a44 +
                a.a14 * a.a32 * a.a41 - a.a11 * a.a32 * a.a44 -
                a.a12 * a.a34 * a.a41 - a.a14 * a.a31 * a.a42) /
               det,

        .a33 = (a.a11 * a.a22 * a.a44 + a.a12 * a.a24 * a.a41 +
                a.a14 * a.a21 * a.a42 - a.a11 * a.a24 * a.a42 -
                a.a12 * a.a21 * a.a44 - a.a14 * a.a22 * a.a41) /
               det,

        .a34 = (a.a11 * a.a24 * a.a32 + a.a12 * a.a21 * a.a34 +
                a.a14 * a.a22 * a.a31 - a.a11 * a.a22 * a.a34 -
                a.a12 * a.a24 * a.a31 - a.a14 * a.a21 * a.a32) /
               det,

        .a41 = (a.a21 * a.a33 * a.a42 + a.a22 * a.a31 * a.a43 +
                a.a23 * a.a32 * a.a41 - a.a21 * a.a32 * a.a43 -
                a.a22 * a.a33 * a.a41 - a.a23 * a.a31 * a.a42) /
               det,

        .a42 = (a.a11 * a.a32 * a.a43 + a.a12 * a.a33 * a.a41 +
                a.a13 * a.a31 * a.a42 - a.a11 * a.a33 * a.a42 -
                a.a12 * a.a31 * a.a43 - a.a13 * a.a32 * a.a41) /
               det,

        .a43 = (a.a11 * a.a23 * a.a42 + a.a12 * a.a21 * a.a43 +
                a.a13 * a.a22 * a.a41 - a.a11 * a.a22 * a.a43 -
                a.a12 * a.a23 * a.a41 - a.a13 * a.a21 * a.a42) /
               det,

        .a44 = (a.a11 * a.a22 * a.a33 + a.a12 * a.a23 * a.a31 +
                a.a13 * a.a21 * a.a32 - a.a11 * a.a23 * a.a32 -
                a.a12 * a.a21 * a.a33 - a.a13 * a.a22 * a.a31) /
               det};
}
inline Mat4 mat4_translation(Vec3 t)
{
    return (Mat4){
        .a11 = 1.0f, .a12 = 0.0f, .a13 = 0.0f, .a14 = t.x, .a21 = 0.0f, .a22 = 1.0f, .a23 = 0.0f, .a24 = t.y, .a31 = 0.0f, .a32 = 0.0f, .a33 = 1.0f, .a34 = t.z, .a41 = 0.0f, .a42 = 0.0f, .a43 = 0.0f, .a44 = 1.0f};
}

inline Mat4 mat4_scale(Vec3 a)
{
    return (Mat4){
        .a11 = a.x, .a12 = 0.0f, .a13 = 0.0f, .a14 = 0.0f, .a21 = 0.0f, .a22 = a.y, .a23 = 0.0f, .a24 = 0.0f, .a31 = 0.0f, .a32 = 0.0f, .a33 = a.z, .a34 = 0.0f, .a41 = 0.0f, .a42 = 0.0f, .a43 = 0.0f, .a44 = 1.0f};
}

inline Mat4 mat4_scalef(float a)
{
    return (Mat4){
        .a11 = a, .a12 = 0.0f, .a13 = 0.0f, .a14 = 0.0f, .a21 = 0.0f, .a22 = a, .a23 = 0.0f, .a24 = 0.0f, .a31 = 0.0f, .a32 = 0.0f, .a33 = a, .a34 = 0.0f, .a41 = 0.0f, .a42 = 0.0f, .a43 = 0.0f, .a44 = 1.0f};
}

inline Mat4 mat4_rotation(Quat q2)
{
    Mat4 matrix;
    q2 = quat_normalize(q2);

    Quat q;
    q.w = q2.w;
    q.x = q2.x;
    q.y = q2.y;
    q.z = q2.z;

    q = quat_normalize(q);

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    matrix.a11 = 1.0f - 2.0f * (yy + zz);
    matrix.a12 = 2.0f * (xy - wz);
    matrix.a13 = 2.0f * (xz + wy);
    matrix.a14 = 0.0f;

    matrix.a21 = 2.0f * (xy + wz);
    matrix.a22 = 1.0f - 2.0f * (xx + zz);
    matrix.a23 = 2.0f * (yz - wx);
    matrix.a24 = 0.0f;

    matrix.a31 = 2.0f * (xz - wy);
    matrix.a32 = 2.0f * (yz + wx);
    matrix.a33 = 1.0f - 2.0f * (xx + yy);
    matrix.a34 = 0.0f;

    matrix.a41 = 0.0f;
    matrix.a42 = 0.0f;
    matrix.a43 = 0.0f;
    matrix.a44 = 1.0f;

    return matrix;
}

inline Mat4 mat4_perspective(float fov, float aspect, float near, float far)
{
    float tanHalfFov = tanf(fov * 0.5f);
    float rangeInv = 1.0f / (near - far);

    return (Mat4){
        .a11 = 1.0f / (aspect * tanHalfFov), .a12 = 0.0f, .a13 = 0.0f, .a14 = 0.0f, .a21 = 0.0f, .a22 = 1.0f / tanHalfFov, .a23 = 0.0f, .a24 = 0.0f, .a31 = 0.0f, .a32 = 0.0f, .a33 = (near + far) * rangeInv, .a34 = 2.0f * near * far * rangeInv, .a41 = 0.0f, .a42 = 0.0f, .a43 = -1.0f, .a44 = 0.0f};
}

inline Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near, float far)
{
    return (Mat4){
        .a11 = 2.0f / (right - left), .a12 = 0.0f, .a13 = 0.0f, .a14 = -(right + left) / (right - left), .a21 = 0.0f, .a22 = 2.0f / (top - bottom), .a23 = 0.0f, .a24 = -(top + bottom) / (top - bottom), .a31 = 0.0f, .a32 = 0.0f, .a33 = -2.0f / (far - near), .a34 = -(far + near) / (far - near), .a41 = 0.0f, .a42 = 0.0f, .a43 = 0.0f, .a44 = 1.0f};
}

inline Mat4 mat4_lookat(Vec3 eye, Vec3 center, Vec3 up)
{
    Vec3 yaxis = vec3_norm(vec3_sub(eye, center));
    Vec3 xaxis = vec3_norm(vec3_cross(up, yaxis));
    Vec3 zaxis = vec3_cross(yaxis, xaxis);
    return (Mat4){.a11 = xaxis.x, .a12 = xaxis.y, .a13 = xaxis.z, .a14 = -vec3_dot(xaxis, eye), .a21 = zaxis.x, .a22 = zaxis.y, .a23 = zaxis.z, .a24 = -vec3_dot(zaxis, eye), .a31 = yaxis.x, .a32 = yaxis.y, .a33 = yaxis.z, .a34 = -vec3_dot(yaxis, eye), .a41 = 0.0f, .a42 = 0.0f, .a43 = 0.0f, .a44 = 1.0f};
}

inline Mat4 mat4_mul(Mat4 a, Mat4 b)
{
    Mat4 result;

    result.a11 = a.a11 * b.a11 + a.a12 * b.a21 + a.a13 * b.a31 + a.a14 * b.a41;
    result.a12 = a.a11 * b.a12 + a.a12 * b.a22 + a.a13 * b.a32 + a.a14 * b.a42;
    result.a13 = a.a11 * b.a13 + a.a12 * b.a23 + a.a13 * b.a33 + a.a14 * b.a43;
    result.a14 = a.a11 * b.a14 + a.a12 * b.a24 + a.a13 * b.a34 + a.a14 * b.a44;

    result.a21 = a.a21 * b.a11 + a.a22 * b.a21 + a.a23 * b.a31 + a.a24 * b.a41;
    result.a22 = a.a21 * b.a12 + a.a22 * b.a22 + a.a23 * b.a32 + a.a24 * b.a42;
    result.a23 = a.a21 * b.a13 + a.a22 * b.a23 + a.a23 * b.a33 + a.a24 * b.a43;
    result.a24 = a.a21 * b.a14 + a.a22 * b.a24 + a.a23 * b.a34 + a.a24 * b.a44;

    result.a31 = a.a31 * b.a11 + a.a32 * b.a21 + a.a33 * b.a31 + a.a34 * b.a41;
    result.a32 = a.a31 * b.a12 + a.a32 * b.a22 + a.a33 * b.a32 + a.a34 * b.a42;
    result.a33 = a.a31 * b.a13 + a.a32 * b.a23 + a.a33 * b.a33 + a.a34 * b.a43;
    result.a34 = a.a31 * b.a14 + a.a32 * b.a24 + a.a33 * b.a34 + a.a34 * b.a44;

    result.a41 = a.a41 * b.a11 + a.a42 * b.a21 + a.a43 * b.a31 + a.a44 * b.a41;
    result.a42 = a.a41 * b.a12 + a.a42 * b.a22 + a.a43 * b.a32 + a.a44 * b.a42;
    result.a43 = a.a41 * b.a13 + a.a42 * b.a23 + a.a43 * b.a33 + a.a44 * b.a43;
    result.a44 = a.a41 * b.a14 + a.a42 * b.a24 + a.a43 * b.a34 + a.a44 * b.a44;

    return result;
}

inline Mat4 mat4_scaleby(Mat4 a, float b)
{
    return (Mat4){
        .a11 = a.a11 * b, .a12 = a.a12 * b, .a13 = a.a13 * b, .a14 = a.a14 * b, .a21 = a.a21 * b, .a22 = a.a22 * b, .a23 = a.a23 * b, .a24 = a.a24 * b, .a31 = a.a31 * b, .a32 = a.a32 * b, .a33 = a.a33 * b, .a34 = a.a34 * b, .a41 = a.a41 * b, .a42 = a.a42 * b, .a43 = a.a43 * b, .a44 = a.a44 * b};
}

inline Mat4 mat4_div(Mat4 a, Mat4 b)
{
    return mat4_mul(a, mat4_invert(b));
}

inline void mat4_debug(Mat4 m)
{
    size_t start = (size_t)(&m);

    printf("mat4(\n");
    for (int i = 0; i < 16; i++)
    {
        float r = *((float *)start);
        printf("%.2f,     ", r);
        if ((i + 1) % 4 == 0)
            printf("\n");

        start += sizeof(float);
    }
    printf(")");
}

*/