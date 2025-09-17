$input  a_position, a_color0, a_normal
$output v_color0, v_worldPos, v_normal

#include <bgfx_shader.sh>

mat3 mtxTranspose3x3(mat3 m)
{
    return mat3(
        vec3(m[0][0], m[1][0], m[2][0]),
        vec3(m[0][1], m[1][1], m[2][1]),
        vec3(m[0][2], m[1][2], m[2][2])
    );
}


mat3 mtxInverse3x3(mat3 m)
{
    vec3 a = m[0];
    vec3 b = m[1];
    vec3 c = m[2];

    vec3 r0 = cross(b, c);
    vec3 r1 = cross(c, a);
    vec3 r2 = cross(a, b);

    float det = dot(a, r0);


    const float eps = 1e-8;
    float invDet = abs(det) > eps ? (1.0 / det) : 0.0;

   
    return mat3(
        r0 * invDet,
        r1 * invDet,
        r2 * invDet
    );
}

mat3 mtxNormalMatrix(mat4 model)
{
    mat3 m3 = mat3(model);
    return mtxTranspose3x3(mtxInverse3x3(m3));
}


void main()
{
    // world position
    vec3 wp = mul(u_model[0], vec4(a_position, 1.0)).xyz;
    v_worldPos = wp;

    // pass color through
    v_color0 = a_color0;

    // transform normal (ok for uniform scale; for non-uniform use proper normal matrix)
    v_normal = normalize(mtxNormalMatrix(u_model[0]) * a_normal);

    // clip-space position
    gl_Position = mul(u_viewProj, vec4(wp, 1.0));
}
