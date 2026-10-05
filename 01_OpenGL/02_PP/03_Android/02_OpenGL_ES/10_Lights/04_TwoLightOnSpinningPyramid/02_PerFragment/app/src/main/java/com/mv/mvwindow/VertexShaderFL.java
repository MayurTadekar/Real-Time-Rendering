//
//  VertexShader.java
//  
//
//  Created by Mayur Tadekar on 18/11/25.
//

package com.mv.mvwindow;

public class VertexShaderFL
{
    private final static String vertexShader = String.format(
"#version 320 es"                                                            +
"\n"                                                                        +
"in vec4 vPosition;"                                                        +
"in vec3 vNormal;"                                                            +
"uniform mat4 u_mMatrix;"                                                    +
"uniform mat4 u_vMatrix;"                                                    +
"uniform mat4 u_pMatrix;"                                                    +
"uniform mediump int toggleLight;"                                                +
"uniform vec3 u_la[2];"                                                        +
"uniform vec3 u_ld[2];"                                                        +
"uniform vec3 u_ls[2];"                                                        +
"uniform vec3 u_ka;"                                                        +
"uniform vec3 u_kd;"                                                        +
"uniform vec3 u_ks;"                                                        +
"uniform float u_kshine;"                                                    +
"uniform vec4 u_lightPosition[2];"                                                +
"out vec3 view_vector;"                                                        +
"out vec3 light_direction[2];"                                                    +
"out vec3 t_norm;"                                                            +
"void main(void)"                                                            +
"{"                                                                            +
"    if( toggleLight == 1 )"                                                        +
"    {    "                                                                    +
"        vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;"                +
"        t_norm = normalize(mat3(u_vMatrix * u_mMatrix ) * vNormal);"    +
"        view_vector = normalize(-eye_coord.xyz);"                            +
"        for( int i = 0 ; i < 2; i++ ) "+
"        {    "+
"          light_direction[i] = normalize(vec3(u_lightPosition[i] - eye_coord));"    +
"        }    "                                                                    +
"    }    "                                                                    +
"    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;    "        +
"        "                                                                    +
"}"
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(vertexShader);
    }
}
