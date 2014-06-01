#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vLightDirection;

out vec4 oColor;

uniform sampler2D uTexture;
uniform float uOpacity;
uniform vec3 uColor;
uniform vec3 uViewVector;
uniform mat4 uViewMatrix;

void main()
{
    vec3 worldCameraToVertex = vWorldPos - uViewVector;
    vec3 viewCameraToVertex = (uViewMatrix * vec4(worldCameraToVertex, 0.0)).xyz;
    viewCameraToVertex = normalize(worldCameraToVertex);
    vec3 normal = normalize(vNormal);

    float intensity = pow(0.4 + dot(normal, viewCameraToVertex), 1.4);

    vec3 color = texture(uTexture, vTexCoord).rgb;
    color = uColor;

    float lightingTerm = dot( normalize(vNormal), viewCameraToVertex );
    lightingTerm = 0.5*lightingTerm + 0.5;
    color *= lightingTerm;

    if(uOpacity == 0.9) {
        oColor = vec4(uColor, uOpacity);
    } else {
        vec4 sum = vec4(0.0);
        oColor = vec4(uColor, intensity);
    }
}
