#version 450 core			
							
layout (triangles) in;	
layout (triangle_strip, max_vertices = 9) out;	

uniform mat4 u_mMatrix;
uniform mat4 u_vMatrix;
uniform mat4 u_pMatrix;

void main(void)				
{							
   for(int i = 0; i < 3; ++i)	
   {	
       gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * (gl_in[i].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));	
       EmitVertex();	
   	
       gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * (gl_in[i].gl_Position + vec4(-1.0, -1.0, 0.0, 0.0));	
       EmitVertex();	
   	
       gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * (gl_in[i].gl_Position + vec4(1.0, -1.0, 0.0, 0.0));	
       EmitVertex();	
       EndPrimitive();	
   }	
}
