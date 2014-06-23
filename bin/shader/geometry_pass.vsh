#version 330
                                                                                    
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;
                                        
out vec2 TexCoord0;                                                                 
out vec3 Normal0;                                                                   
out vec3 WorldPos0;                                                                 


void main()
{       
    gl_Position    = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(Position, 1.0);
    TexCoord0      = TexCoord;                  
    Normal0        = uNormalMatrix * Normal;
    WorldPos0      = (uViewMatrix * uModelMatrix * vec4(Position, 1.0)).xyz;
}
