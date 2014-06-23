#version 150

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D renderedTexture;

void main(){
    vec3 color = texture( renderedTexture, vTexCoord ).rgb;
    FragColor = vec4( color , 1.0 ) ;
}
