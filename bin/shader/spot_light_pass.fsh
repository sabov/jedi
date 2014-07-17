#version 330

//Positional light structure
struct sSpotLight
{
        vec4 Position               ;
        vec4 SpotDirection          ;
        vec4 AmbientColor           ;
        vec4 DiffuseColor           ;
        vec4 SpecularColor          ;
        float ConstantAttenuation   ;
        float LinearAttenuation     ;
        float ExponentialAttenuation;
        float SpotCutOff1           ;
        float SpotCutOff2           ;
};

uniform sampler2D   gPositionMap;
uniform sampler2D   gColorMap;
uniform sampler2D   gNormalMap;
uniform vec3        g_EyeWorldPos;
uniform vec2        gScreenSize;
uniform sSpotLight  SpotLight;
uniform mat4        gLightTransform;

vec4 phong_spot_light(  in vec4 matAmbient,
                        in vec4 matDiffuse,
                        in vec4 matSpecular,
                        in float shininess,
                        in vec3 normal,
                        in vec3 eyeVec,
                        in vec3 lightDirection,
                        in sSpotLight light)
{
    float d = length(lightDirection);
    float attenuation_factor = 1.0 / (light.ConstantAttenuation + (light.LinearAttenuation * d) + (light.ExponentialAttenuation * d * d)) ;

    vec4 final_color = light.AmbientColor * matAmbient;

    vec3 N = normalize(normal) ;
    vec3 L = normalize(lightDirection);
    //vec4 D = normalize(gLightTransform * light.SpotDirection);
    vec4 D = normalize(light.SpotDirection);

    float cos_cur_angle = dot(L, D.xyz);

    float cos_inner_cone_angle = light.SpotCutOff1;

    float cos_inner_minus_outer_angle = cos_inner_cone_angle - light.SpotCutOff2;

    float spot = 0.0;
    spot = clamp((cos_cur_angle - light.SpotCutOff2) / cos_inner_minus_outer_angle, 0.0, 1.0);

    float lambertTerm = dot(N,L);

    if(lambertTerm > 0.0)
    {
        final_color += light.DiffuseColor * matDiffuse * lambertTerm * spot;

        vec3 E = normalize(eyeVec);
        vec3 R = reflect(-L, N);
        float specular = 0.0;
        if (shininess > 0.0)
        {
            specular = pow( max(dot(R, E), 0.0), shininess );
            final_color += light.SpecularColor * matSpecular * specular * spot;
        }
    }
   return final_color * attenuation_factor;
}


vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

layout (location = 0) out vec4 FragColor;

void main()
{
    vec2 TexCoord = CalcTexCoord();
    vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
    vec3 Color = texture(gColorMap, TexCoord).xyz;
    vec3 Normal = texture(gNormalMap, TexCoord).xyz;

    vec3 VertexToEye = g_EyeWorldPos- WorldPos;
    vec4 LightPos = gLightTransform * SpotLight.Position ;
    vec3 LightDirection = LightPos.xyz - WorldPos;

    FragColor = phong_spot_light( vec4(0.0), vec4(Color, 1.0), vec4(0.0), 0.0, Normal, VertexToEye, LightDirection, SpotLight );
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
