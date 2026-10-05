#version 450 core

in	vec4	vPosition;	
in	vec3	vNormal;		

uniform		mat4	u_mMatrix;
uniform		mat4	u_vMatrix;
uniform		mat4	u_pMatrix;

uniform		int 	u_lKey;

uniform 	vec4 	u_lightPosition;

out			vec3 	view_vector; 
out			vec3 	light_direction;
out 		vec3 	transformed_normal;

void main(void)	
{					
	
	if( 1 == u_lKey ) {
		vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;
		transformed_normal = mat3(u_vMatrix * u_mMatrix) * vNormal;
		light_direction = vec3(u_lightPosition - eye_coord );
		view_vector = -eye_coord.xyz;
	}

	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;
}
