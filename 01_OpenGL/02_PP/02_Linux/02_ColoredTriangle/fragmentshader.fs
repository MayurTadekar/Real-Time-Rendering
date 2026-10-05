#version 450 core

out vec4 fragColor;

in vec3 ovColor;

void main(void)
{
    fragColor = vec4(ovColor, 1.0f);
}
