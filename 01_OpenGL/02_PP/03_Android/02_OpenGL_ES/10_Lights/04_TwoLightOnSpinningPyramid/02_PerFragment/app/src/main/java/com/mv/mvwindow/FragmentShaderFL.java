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
   "in vec3 light_direction[2];"                        +
   "in vec3 t_norm;"                                +
   "uniform mediump int toggleLight;"                    +
   "uniform vec3 u_la[2];"                            +
   "uniform vec3 u_ld[2];"                            +
   "uniform vec3 u_ls[2];"                            +
   "uniform vec3 u_ka;"                            +
   "uniform vec3 u_kd;"                            +
   "uniform vec3 u_ks;"                            +
   "uniform float u_kshine;"                        +
   "out vec4 FragColor;"                            +
   "vec4 ads_light;"                                +
   "void main(void)"                                +
   "{"                                                +
   "    if( toggleLight == 1 )    "                        +
   "    {    "                                        +
   "        for( int i = 0 ; i < 2; i++ ) "+
   "        {    "+
   "            vec3 reflection_vec = reflect( -light_direction[i], t_norm);    "    +
   "            vec3 ambient = u_la[i] * u_ka;    "            +
   "            vec3 diffuse = u_ld[i] * u_kd * max( dot( light_direction[i], t_norm), 0.0f );    "    +
   "            vec3 specular = u_ls[i] * u_ks * pow( max( dot( reflection_vec, view_vector), 0.0f), u_kshine);    "    +
   "            FragColor = FragColor + vec4( (ambient + diffuse + specular) , 1.0f);    "    +
   "        }    "                                                +
   "    }    "                                                +
   "    else    "                                            +
   "    {    "                                                +
   "        ads_light = vec4(1.0f);    "        +
   "        FragColor = ads_light;"                    +
   "    }    "                                                +
   "}"
                                                               );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
