#version 450 core	

in vec4 vPosition;
in vec2 vTexcoord;

out vec2 ovTexcoord;

uniform mat4 u_mMatrix;
uniform mat4 u_vMatrix;
uniform mat4 u_pMatrix;

void main(void)	
{					
    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;
    ovTexcoord = vTexcoord;
}
