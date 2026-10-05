#version 450 core

out vec4 fragColor;

in vec2 ovTexcoord;

uniform sampler2D smileySampler;

void main(void)
{
    fragColor = texture(smileySampler, vec2(-ovTexcoord.x, ovTexcoord.y));
}
