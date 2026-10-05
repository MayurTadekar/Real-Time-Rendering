#version 450 core

in	vec4	vPosition;	
in	vec3	vNormal;		

uniform		mat4	u_mMatrix;
uniform		mat4	u_vMatrix;
uniform		mat4	u_pMatrix;

uniform		int 	u_lKey;

uniform		vec3	u_lightAmbient;
uniform		vec3	u_lightDiffuse;
uniform		vec3	u_lightSpecular;
uniform 	vec4 	u_lightPosition;

uniform 	vec3 	u_materialAmbient;
uniform 	vec3 	u_materialDiffuse;
uniform 	vec3 	u_materialSpecular;
uniform 	float 	u_materialShininess;

out 		vec3 	phong_light;

void main(void)	
{					
	
	if( 1 == u_lKey ) {
		vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;
		vec3 transformed_normal = normalize( mat3(u_vMatrix * u_mMatrix) * vNormal );
		vec3 light_direction = normalize( vec3(u_lightPosition - eye_coord ) );
		vec3 reflection_vector = reflect( -light_direction , transformed_normal );
		vec3 view_vector = normalize(-eye_coord.xyz);
		vec3 intensity_ambient = u_lightAmbient * u_materialAmbient;
		vec3 intensity_diffuse = u_lightDiffuse * u_materialDiffuse * max( dot(light_direction, transformed_normal), 0.0f);
		vec3 intensity_specular = u_lightSpecular * u_materialSpecular * pow( max( dot (reflection_vector, view_vector ), 0.0f ), u_materialShininess);
		phong_light = intensity_ambient + intensity_diffuse + intensity_specular;
	} else {
		phong_light = vec3(1.0f);
	}

	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;
}
