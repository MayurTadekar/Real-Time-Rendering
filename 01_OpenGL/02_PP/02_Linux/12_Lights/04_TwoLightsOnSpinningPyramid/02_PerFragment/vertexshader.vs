#version 450 core
																			
in vec4 vPosition;															
in vec3 vNormal;															

uniform mat4 u_mMatrix;                                             	
uniform mat4 u_vMatrix;                                             		
uniform mat4 u_pMatrix;                                            

uniform int  u_lKey;                                                   
uniform vec4  u_lightPosition[2];                                             

out vec3 light_direction[2];
out vec3 transformed_normal;
out vec3 view_vector;

void main(void)                                                             
{                                                                             
	if( u_lKey == 1 )                                                     
	{                                                                             
		for( int i = 0 ; i < 2; i++)       
		{                                      
			vec4 eye_coord =  u_vMatrix * u_mMatrix * vPosition;                                                    
			transformed_normal = mat3(u_vMatrix * u_mMatrix) * vNormal;                                             
			light_direction[i] = vec3(u_lightPosition[i] - eye_coord);                                                           
			view_vector = -eye_coord.xyz;                                                                                  
		}      
	}                                                                                                              	
	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;                                   	
};
