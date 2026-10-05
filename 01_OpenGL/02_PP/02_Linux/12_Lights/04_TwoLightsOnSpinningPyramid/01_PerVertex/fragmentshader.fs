#version 450 core

out		vec4	Fragcolor;

in 		vec3 	phong_light;

void main(void)
{
	Fragcolor = vec4(phong_light, 1.0f);
}
