$input  a_position, a_color0, a_normal
$output v_color0, v_worldPos, v_normal

#include <bgfx_shader.sh>
uniform vec4 u_TimeDeltaMouseXY; // x=time, y=dt, zw=mouse

mat3 mtxTranspose3x3(mat3 m) {
    return mat3(
        vec3(m[0][0], m[1][0], m[2][0]),
        vec3(m[0][1], m[1][1], m[2][1]),
        vec3(m[0][2], m[1][2], m[2][2])
    );
}

mat3 mtxInverse3x3(mat3 m) {
    vec3 a = m[0], b = m[1], c = m[2];
    vec3 r0 = cross(b, c);
    vec3 r1 = cross(c, a);
    vec3 r2 = cross(a, b);
    float det = dot(a, r0);
    const float eps = 1e-8;
    float invDet = abs(det) > eps ? (1.0 / det) : 0.0;
    return mat3(r0 * invDet, r1 * invDet, r2 * invDet);
}

mat3 mtxNormalMatrix(mat4 model) {
    return mtxTranspose3x3(mtxInverse3x3(mat3(model)));
}

void main()
{
    // object-space xz (height field centered with a small offset)
    vec2 p = a_position.xz + vec2(0.75, -0.75);

    // wave params
    const float A = 0.02;
    const float k = 50.0;
    const float speed = 2.0;

    float t = u_TimeDeltaMouseXY.x;
    float r = length(p);

    // height (along object-space Y)
    float h = A * sin(k * r - speed * t);

    // gradient magnitude factor: d/dx h = A*k*cos(...) * (x/r), d/dz h = A*k*cos(...) * (z/r)
    float g = (r > 0.0001) ? (A * k * cos(k * r - speed * t) / r) : 0.0;
    vec2 dfdxz = g * p;

    // correct object-space geometric normal: (-∂h/∂x, 1, -∂h/∂z)
    vec3 n_obj = normalize(vec3(-dfdxz.x, 1.0, -dfdxz.y));

    // displace vertex in object space, then transform to world/clip
    vec3 osPos = a_position;
    osPos.y = h;

    vec3 wp = mul(u_model[0], vec4(osPos, 1.0)).xyz;
    v_worldPos = wp;

    // transform normal with proper normal matrix (no reflect)
    v_normal = normalize(mul(mtxNormalMatrix(u_model[0]), n_obj));

    // pass color through
    v_color0 = a_color0;

    // clip-space
    gl_Position = mul(u_viewProj, vec4(wp, 1.0));
}
