#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vLightDirection;

out vec4 oColor;

//uniform sampler2D uTexture;

void main()
{
    //vec3 color = texture(uTexture, vTexCoord).rgb;
    vec3 color = vec3(1.0f, 0.0f, 0.0f);

    float lightingTerm = dot( normalize(vNormal), vLightDirection );
    lightingTerm = 0.5*lightingTerm + 0.5;
    color *= lightingTerm;

    oColor = vec4( color, 1.0 );
}
