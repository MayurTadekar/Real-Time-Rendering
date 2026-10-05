//
//  VertexShader.java
//  
//
//  Created by Mayur Tadekar on 18/11/25.
//

package com.mv.mvwindow;

public class FragmentShaderFL
{
    private final static String fragmentShader = String.format(
                                                               "#version 320 es"                                +
                                                               "\n"                                            +
                                                               "precision highp float;"                        +
                                                               "in vec3 view_vector;"                            +
                                                               "in vec3 light_direction;"                        +
                                                               "in vec3 t_norm;"                                +
                                                               "uniform mediump int toggleFlag;"                    +
                                                               "uniform vec3 u_la;"                            +
                                                               "uniform vec3 u_ld;"                            +
                                                               "uniform vec3 u_ls;"                            +
                                                               "uniform vec3 u_ka;"                            +
                                                               "uniform vec3 u_kd;"                            +
                                                               "uniform vec3 u_ks;"                            +
                                                               "uniform float u_kshine;"                        +
                                                               "out vec4 FragColor;"                            +
                                                               "vec3 ads_light;"                                +
                                                               "void main(void)"                                +
                                                               "{"                                                +
                                                               "    if( toggleFlag == 1 )    "                        +
                                                               "    {    "                                        +
                                                               "        vec3 reflection_vec = reflect( -light_direction, t_norm);    "    +
                                                               "        vec3 ambient = u_la * u_ka;    "            +
                                                               "        vec3 diffuse = u_ld * u_kd * max( dot( light_direction, t_norm), 0.0f );    "    +
                                                               "        vec3 specular = u_ls * u_ks * pow( max( dot( reflection_vec, view_vector), 0.0f), u_kshine);    "    +
                                                               "        ads_light = ambient + diffuse + specular;    "    +
                                                               "    }    "                                                +
                                                               "    else    "                                            +
                                                               "    {    "                                                +
                                                               "        ads_light = vec3( 1.0f, 1.0f, 1.0f);    "        +
                                                               "    }    "                                                +
                                                               "    FragColor = vec4(ads_light, 1.0f);"                    +
                                                               "}"
                                                               );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
