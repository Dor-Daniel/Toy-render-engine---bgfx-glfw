$input v_color0, v_worldPos, v_normal
#include <bgfx_shader.sh>

uniform vec4 u_lightDirColor;      // xyz = light dir (from light), w = intensity
uniform vec4 u_albedo;             // rgb = base color
uniform vec4 u_cameraPosShininess;

void main()
{
    float ambient_strength = 0.1;
    vec3  ambient = u_albedo.xyz * ambient_strength;
    float specularStrength = 0.5;

    vec3 viewDir = normalize(u_cameraPosShininess.xyz - v_worldPos);
    vec3 reflectDir = reflect(-u_lightDirColor.xyz, normalize(v_normal)); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_cameraPosShininess.w);
    vec3 specular = specularStrength * spec * u_lightDirColor.w;
    
    float diff = max(dot(normalize(v_normal), normalize(u_lightDirColor.xyz)), 0.0);
    vec3  result = (ambient + diff + specular) * v_color0.rgb;

    gl_FragColor = vec4(result, 1.0);
}
