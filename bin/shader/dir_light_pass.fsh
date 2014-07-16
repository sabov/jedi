#version 330

//Directional light structure
struct sDirectionalLight
{
        vec4 Direction		;
        vec4 AmbientColor	;
        vec4 DiffuseColor	;
        vec4 SpecularColor	;
};

//Positional light structure
struct sPositionalLight
{
        vec4 Position				;
        vec4 AmbientColor			;
        vec4 DiffuseColor			;
        vec4 SpecularColor			;
        float ConstantAttenuation               ;
        float LinearAttenuation                 ;
        float ExponentialAttenuation            ;
};

uniform sampler2D   gPositionMap;
uniform sampler2D   gColorMap;
uniform sampler2D   gNormalMap;
uniform vec3        g_EyeWorldPos;
uniform vec2        gScreenSize;
uniform sDirectionalLight    DirectionalLight;
uniform mat4        gLightTransform;

vec4 phong_directional_light(	in vec4 matAmbient,
                                in vec4 matDiffuse,
                                in vec4 matSpecular,
                                in float shininess,
                                in vec3 normal,
                                in vec3 eyeVec,
                                in sDirectionalLight light)
{
    vec4 final_color = light.AmbientColor * matAmbient;

    vec3 N = normalize(normal) ;
    vec3 L = normalize(light.Direction.xyz);

    float lambertTerm = dot(N,L);

    if(lambertTerm > 0.0)
    {
        final_color += light.DiffuseColor * matDiffuse * lambertTerm;

        vec3 E = normalize(eyeVec);
        vec3 R = reflect(-L, N);
        float specular = 0.0;
        if (shininess > 0.0)
        {
            specular = pow( max(dot(R, E), 0.0), shininess );
        }
        final_color += light.SpecularColor * matSpecular * specular;
    }
    return final_color;
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
    //vec4 LightDirection = gLightTransform * DirectionalLight.Direction ;

    FragColor = phong_directional_light( vec4(Color, 1.0), vec4(Color, 1.0), vec4(0.0), 0.0, Normal, VertexToEye, DirectionalLight );
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
