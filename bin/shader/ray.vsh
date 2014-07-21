#version 330

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat3 uNormalMatrix;

in vec3 aNormal;
in vec3 aPosition;
in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vWorldPos;
out vec3 vLightDirection;

void main()
{
    vNormal       = uNormalMatrix * aNormal;
    vTexCoord     = aTexCoord;
    vec4 worldPos = uModelMatrix * vec4(aPosition, 1.0);
    vWorldPos     = worldPos.xyz;
    gl_Position   = uProjectionMatrix * uViewMatrix * worldPos;

    vLightDirection = vec3( uViewMatrix * vec4(0,1,0,0) );
}
