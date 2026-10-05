//
//  VertexShader.java
//  
//
//  Created by Mayur Tadekar on 18/11/25.
//

package com.mv.mvwindow;

public class FragmentShader
{
    private final static String fragmentShader = String.format(
"#version 320 es"                                +
"\n"                                            +
"precision highp float;"                        +
"out vec4 FragColor;"                            +
"uniform vec3 u_la[3];"                                                        +
"uniform vec3 u_ld[3];"                                                        +
"uniform vec3 u_ls[3];"                                                        +
"uniform vec3 u_ka;"                                                        +
"uniform vec3 u_kd;"                                                        +
"uniform vec3 u_ks;"                                                        +
"uniform float u_kshine;"                                       +
"in vec3 ads_light;"                            +
"in vec3 t_norm;"                            +
"in vec3 light_direction[3];"                            +
"in vec3 view_vector;"                            +
"uniform mediump int toggleFlag;"                                                +
"void main(void)"                                +
"{"                                                +
"    if( toggleFlag == 2 )    "                        +
"    {    "                                        +
"       for(int le = 0 ; le < 3; ++le)"     +
"       {    "                                        +
"           vec3 reflection_vec = reflect( -light_direction[le], t_norm);    "    +
"           vec3 ambient = u_la[le] * u_ka;    "            +
"           vec3 diffuse = u_ld[le] * u_kd * max( dot( light_direction[le], t_norm), 0.0f );    "    +
"           vec3 specular = u_ls[le] * u_ks * pow( max( dot( reflection_vec, view_vector), 0.0f), u_kshine);    "    +
"           FragColor = FragColor + vec4( (ambient + diffuse + specular) , 1.0f);"                +
"       }    "                                                +
"    }    "                                                +
"    else  if( toggleFlag == 1 )"                            +
"    {    "                                                +
"        FragColor = vec4(ads_light, 1.0f);"                +
"    }    "                                                +
"    else  "                                                +
"    {    "                                                +
"        FragColor = vec4(ads_light, 1.0f);"                +
"    }    "                                                +
"}"
                                                               );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
