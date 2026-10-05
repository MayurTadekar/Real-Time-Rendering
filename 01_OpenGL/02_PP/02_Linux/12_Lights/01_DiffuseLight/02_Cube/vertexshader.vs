#version 450 core	

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 u_mMatrix;
uniform mat4 u_vMatrix;
uniform mat4 u_pMatrix;

uniform		int 	u_lKey;
uniform		vec3	u_lightDiffuse;
uniform 	vec3 	u_materialDiffuse;
uniform 	vec4 	u_lightPosition;
out 		vec3 	diffuse_light;

void main(void)	
{					
    if( 1 == u_lKey ) {
		vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;
		mat3 normal_matrix = mat3( transpose ( inverse( u_vMatrix * u_mMatrix ) ) );
		vec3 t_norm = normalize(normal_matrix * vNormal);
		vec3 S = normalize( vec3(u_lightPosition - eye_coord ) );
		diffuse_light = u_lightDiffuse * u_materialDiffuse * max( dot( S, t_norm ), 0.0f );
	}

    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;
}
