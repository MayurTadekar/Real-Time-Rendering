#version 450 core

in	vec4	vPosition;	
in	vec3	vNormal;		

uniform		mat4	u_mMatrix;
uniform		mat4	u_vMatrix;
uniform		mat4	u_pMatrix;

uniform		int 	u_lKey;

uniform		vec3	u_lightAmbient[2];
uniform		vec3	u_lightDiffuse[2];
uniform		vec3	u_lightSpecular[2];
uniform 	vec4 	u_lightPosition[2];

uniform 	vec3 	u_materialAmbient;
uniform 	vec3 	u_materialDiffuse;
uniform 	vec3 	u_materialSpecular;
uniform 	float 	u_materialShininess;

out 		vec3 	phong_light;
out			vec3 	view_vector; 
out 		vec3 	transformed_normal;
out			vec3 	light_direction[2];

vec3 	intensity_ambient[2];
vec3 	reflection_vector[2];
vec3 	intensity_diffuse[2];
vec3 	intensity_specular[2];

void main(void)	
{					
	
	if( 1 == u_lKey ) {
		vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;
		transformed_normal = normalize( mat3(u_vMatrix * u_mMatrix) * vNormal );
		view_vector = normalize(-eye_coord.xyz);
		for(int i = 0 ; i < 2; i++) {
			light_direction[i] = normalize( vec3(u_lightPosition[i] - eye_coord ) );
			reflection_vector[i] = reflect( -light_direction[i], transformed_normal );
			intensity_ambient[i] = u_lightAmbient[i] * u_materialAmbient;
			intensity_diffuse[i] = u_lightDiffuse[i] * u_materialDiffuse * max( dot(light_direction[i], transformed_normal), 0.0f);
			intensity_specular[i] = u_lightSpecular[i] * u_materialSpecular * pow( max( dot (reflection_vector[i], view_vector ), 0.0f ), u_materialShininess);
			phong_light = phong_light + intensity_ambient[i] + intensity_diffuse[i] + intensity_specular[i] ;
		}
	} else {
		phong_light = vec3(1.0f);
	}


	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;
}