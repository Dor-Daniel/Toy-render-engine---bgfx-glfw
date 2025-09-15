$input v_color0
uniform vec4 u_resolution;
#include <bgfx_shader.sh>

void main()
{
    vec2 uv = 2. * gl_FragCoord.xy / u_resolution.xy-1.;
    uv.x *= u_resolution.y / u_resolution.x;
    gl_FragColor = v_color0;
}
