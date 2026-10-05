#version 450 core

out		vec4	Fragcolor;

in 		vec3 		transformed_normal;
in 		vec3 		light_direction;
in 		vec3 		view_vector;

vec3 	phong_ads_light;

uniform		vec3	u_lightAmbient;
uniform		vec3	u_lightDiffuse;
uniform		vec3	u_lightSpecular;

uniform 	vec3 	u_materialAmbient;
uniform 	vec3 	u_materialDiffuse;
uniform 	vec3 	u_materialSpecular;
uniform 	float 	u_materialShininess;

uniform		int 	u_lKey;

void main(void)
{
	if( u_lKey == 1 ) 
	{  
	vec3 normalized_transformed_normal = normalize(transformed_normal);
	vec3 normalized_light_direction = normalize(light_direction);
	vec3 normalized_view_vector = normalize(view_vector);
	vec3 reflection_vector = reflect( -normalized_light_direction, normalized_transformed_normal);
	vec3 intensity_ambient = u_lightAmbient * u_materialAmbient;
	vec3 intensity_diffuse = u_lightDiffuse * u_materialDiffuse * max( dot(normalized_light_direction, normalized_transformed_normal), 0.0f);
	vec3 intensity_specular = u_lightSpecular * u_materialSpecular * pow( max( dot( reflection_vector, normalized_view_vector ), 0.0f), u_materialShininess);
	phong_ads_light = intensity_ambient + intensity_diffuse + intensity_specular;
	} else {
		phong_ads_light = vec3(1.0f, 1.0f, 1.0f);
	}
	Fragcolor = vec4(phong_ads_light, 1.0f);
}
