$input v_color0, v_worldPos, v_normal
#include <bgfx_shader.sh>

uniform vec4 u_lightPosIntensity;      // xyz = light dir (from light), w = intensity
uniform vec4 u_albedo;             // rgb = base color
uniform vec4 u_cameraPosShininess;

void main()
{
    float ambient_strength = 0.1;
    vec3  ambient = u_albedo.xyz * ambient_strength;
    float specularStrength = 1.0;

    vec3 lightPos =  u_lightPosIntensity.xyz;
    vec3 lightDir = normalize(lightPos - v_worldPos);

    float dist = length(lightPos - v_worldPos);

    vec3 viewDir = normalize(u_cameraPosShininess.xyz - v_worldPos);
    vec3 reflectDir = reflect(lightDir, normalize(v_normal)); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.01), u_cameraPosShininess.w);
    vec3 specular = specularStrength * spec * u_albedo.xyz;
    
    float diff = max(dot(normalize(v_normal), lightDir), 0.0);
    vec3 diffuse = diff * u_albedo.xyz * u_lightPosIntensity.w ;

    float t =  dot(normalize(v_worldPos), normalize(u_lightPosIntensity.xyz)) ;

    vec3  result = (ambient + diffuse) * (v_color0)   + (1 + t) * specular;

    gl_FragColor = vec4(result, 1.0); 

}
