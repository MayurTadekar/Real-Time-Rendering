#version 450 core

out vec4 fragColor;
    
uniform vec4 lineColor;

void main(void)
{
    fragColor = vec4(lineColor);
}
