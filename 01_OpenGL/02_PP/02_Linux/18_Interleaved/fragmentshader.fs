#version 450 core

out vec4 fragColor;

in vec2 ovTexcoord;
in vec3 ovColor;

uniform sampler2D textureSampler;

void main(void)
{
    fragColor = vec4(ovColor, 1.0) * texture(textureSampler, vec2(-ovTexcoord.x, ovTexcoord.y));
}
