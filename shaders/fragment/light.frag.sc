$input v_color0, v_worldPos, v_normal
#include <bgfx_shader.sh>

uniform vec4 u_lightDirColor;      // xyz = light dir (from light), w = intensity
uniform vec4 u_albedo;             // rgb = base color
uniform vec4 u_cameraPosShininess;

void main()
{


    gl_FragColor = vec4(v_color0.xyz, 1.0);
}
