#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vLightDirection;

out vec4 oColor;

uniform vec4 uColor;

void main()
{
    oColor = uColor;
}