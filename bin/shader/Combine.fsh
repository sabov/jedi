#version 150

uniform sampler2D uSamplerColor;
uniform sampler2D uSamplerNormal;
uniform sampler2D tex0;
uniform vec2 pixelSize;

in vec2 vTexCoord;

out vec4 oColor;

void main()
{
    vec2 curSamplePos;
    vec2 pos=gl_FragCoord.xy*pixelSize;
    pos = vTexCoord;

    float values[9];
    values[0]=0.05;
    values[1]=0.09;
    values[2]=0.11;
    values[3]=0.15;
    values[4]=0.2;
    values[5]=0.15;
    values[6]=0.11;
    values[7]=0.09;
    values[8]=0.05;

    vec4 result = vec4(0,0,0,0);
    curSamplePos=vec2(pos.x - 4.0 * pixelSize.x, pos.y);

    for (int i = 0; i < 9; i++)
    {
        result += texture(uSamplerNormal, curSamplePos) * values[i];
        curSamplePos.x += pixelSize.x;
    }

    //gl_FragColor=vec4(result.xyz,1.0);
    //color = vec4(result.xyz,1.0);

    vec4 color = texture(uSamplerNormal, vTexCoord);
    vec4 color2 = texture(uSamplerNormal, curSamplePos) * 0.8;
    vec4 color3 = texture(tex0, vTexCoord);
    //oColor = color;
    oColor = mix(color3, result, 0.5);

    /*
    vec4 color  = texture(uSamplerColor, vTexCoord);
    vec4 normal = texture(uSamplerNormal, vTexCoord);
    */
}
