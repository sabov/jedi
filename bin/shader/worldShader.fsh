#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vLightDirection;

out vec4 oColor;

#pragma ACGLimport "Noise.glsl"

vec3 proceduralWood( vec3 worldPos );
vec3 proceduralMarble( vec3 worldPos );

void main()
{
    vec3 color;
    if (vWorldPos.y < 0.015f) {
        color = proceduralWood( vWorldPos );
    } else {
        color = proceduralMarble( vWorldPos );
    }

    // very basic fake lighting
    float lightingTerm = dot( normalize(vNormal), vLightDirection );
    lightingTerm = 0.5*lightingTerm + 0.5;
    color *= lightingTerm;

    oColor = vec4( color, 1.0 );
}


// wood
vec3 proceduralWood( vec3 worldPos )
{
    vec4 worldPos4 = vec4( worldPos, 1.0 );
    float Scale = 2.0;
    vec3 LightWood = vec3(0.65, 0.4, 0.3);
    vec3 DarkWood  = vec3(0.4,  0.2, 0.07);
    float RingFreq = 4.0;
    vec3 NoiseScale = vec3(0.5, 0.1, 0.1);
    float Noisiness = 3.0;
    float GrainScale = 27.0;

    vec3 noisevec = vec3( snoise((worldPos)*NoiseScale) )*Noisiness;
    vec3 location = worldPos + noisevec;

    float dist = sqrt(location.x * location.x + location.z * location.z);
    dist *= RingFreq;

    float r = fract(dist + noisevec[0] + noisevec[1] + noisevec[2]) * 2.0;

    if (r > 1.0) r = 2.0 - r;

    vec3 color = mix(LightWood, DarkWood, r);

    return color;
}


vec3 proceduralMarble( vec3 worldPos )
{
    float fac = (worldPos.y+1000)/1000.0;
    vec3 marbleColor = fac * vec3(1.0, 0.95, 0.95);
    vec3 veinColor   = fac * vec3(0.6, 0.5, 0.6);
    float scale = 1.0;
    float mag   = 0.5;
    float yscale = 0.4;
    float size  = 30.0;

    vec3 scaledPos = scale*worldPos;
    vec2 noise = vec2(snoise(scaledPos), snoise(scaledPos.yzx));
    noise = abs(noise-0.5);
    float sum = noise.x + noise.y;
    sum = clamp(mag*sum, 0.0, 1.0);

    float sineval = sin(worldPos.y*yscale + size*yscale*sum) * 0.5 + 0.5;
    return mix(veinColor.rgb, marbleColor.rgb, sineval);
}

