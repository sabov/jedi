#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;
in vec3 vLightDirection;

out vec4 oColor;

uniform vec4 uColor;

void main()
{
    vec2 pixelSize = vec2(1/1000, 1/700);
    int isVertical = 1;
    //oColor = uColor;
    vec2 pos=gl_FragCoord.xy*pixelSize;
    
    // 1 2 1
    // 2 3 2  / 15.0
    // 1 2 1
    
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
    vec4 result;
    if(isVertical==1)
    {
        vec2 curSamplePos=vec2(pos.x,pos.y-4.0*pixelSize.y);
        for(int i=0;i<9;i++)
        {
            result += uColor*values[i];
            curSamplePos.y+=pixelSize.y;
        }
    }else{
        vec2 curSamplePos=vec2(pos.x-4.0*pixelSize.x,pos.y);
        for(int i=0;i<9;i++)
        {
            /*
            result+=texture2D(texture,curSamplePos)*values[i];
            curSamplePos.x+=pixelSize.x;
            */
        }
    }
    oColor = vec4(result.xyz,0.3);
}
