#version 450 core

out vec4 fragColor;

uniform		int 	u_lKey;
in 		vec3 	diffuse_light;

void main(void)
{
	if( 1 == u_lKey ) {
		fragColor = vec4(diffuse_light, 1.0f);
	} else {
		fragColor = vec4(1.0f);
	}
}
