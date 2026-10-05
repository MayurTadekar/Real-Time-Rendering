//
//  VertexShader.java
//  
//
//  Created by Mayur Tadekar on 18/11/25.
//

package com.mv.mvwindow;

public class VertexShader
{
    private final static String vertexShader = String.format(
"#version 320 es"                                                            +
"\n"                                                                        +
"in vec4 vPosition;"                                                        +
"in vec3 vNormal;"                                                           +
"uniform mat4 u_mMatrix;"                                                    +
"uniform mat4 u_vMatrix;"                                                    +
"uniform mat4 u_pMatrix;"                                                    +
"uniform mediump int toggleFlag;"                                                +
"uniform vec3 u_la;"                                                        +
"uniform vec3 u_ld;"                                                        +
"uniform vec3 u_ls;"                                                        +
"uniform vec3 u_ka;"                                                        +
"uniform vec3 u_kd;"                                                        +
"uniform vec3 u_ks;"                                                        +
"uniform float u_kshine;"                                                    +
"uniform vec4 u_lightPosition;"                                              +
"out vec3 ads_light;"                                                        +
"out vec3 t_norm;"                                                        +
"out vec3 light_direction;"                                                        +
"out vec3 view_vector;"                                                        +
"void main(void)"                                                            +
"{"                                                                          +
"    if( toggleFlag == 1 )"                                                      +
"    {    "                                                                  +
"        vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;"                +
"        vec3 t_norm = normalize(mat3(u_vMatrix * u_mMatrix ) * vNormal);"            +
"        vec3 light_direction = normalize(vec3((u_lightPosition * u_vMatrix) - eye_coord));"                +
"        vec3 reflection_vector = reflect(-light_direction, t_norm);"                                +
"        vec3 view_vector = normalize(-eye_coord.xyz);"                                                        +
"        vec3 ambient_light = vec3( u_la * u_ka);        "                                                            +
"        vec3 diffuse_light = vec3( u_ld * u_kd * max ( dot ( light_direction, t_norm), 0.0f));"                            +
"        vec3 specular_light = vec3( u_ls * u_ks * pow(max(dot(reflection_vector, view_vector), 0.0), u_kshine));"        +
"        ads_light = vec3( ambient_light + diffuse_light + specular_light);"                                            +
"    }    "                                                                    +
"   else if( toggleFlag == 2 )"                                                        +
"    {    "                                                                    +
"        vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;"                +
"        t_norm = normalize(mat3(u_vMatrix * u_mMatrix ) * vNormal);"    +
"        light_direction = normalize(vec3((u_lightPosition * u_vMatrix) - eye_coord));"    +
"        view_vector = normalize(-eye_coord.xyz);"                            +
"    }    "    +
"    else    "                                                                +
"    {    "                                                                    +
"        ads_light = vec3( 1.0f, 1.0f, 1.0f);    "                            +
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
