#version 150

in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;

out vec2 vTexCoord;

void main()
{
    vTexCoord = TexCoord;
    gl_Position = vec4(Position, 1.0);
}
