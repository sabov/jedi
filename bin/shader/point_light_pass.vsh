#version 330

layout (location = 0) in vec3 Position;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

void main()
{          
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(Position, 1.0);
}
