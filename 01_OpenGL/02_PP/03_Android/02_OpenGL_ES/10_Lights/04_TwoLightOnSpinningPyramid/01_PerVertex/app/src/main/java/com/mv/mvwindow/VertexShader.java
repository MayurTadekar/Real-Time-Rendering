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
 "\n"                                            +
 "in vec4 vPosition;"                            +
 "in vec4 vColor;"                                +
 "in vec3 vNormal;"                                +
 "uniform mat4 u_mMatrix;"                       +
 "uniform mat4 u_vMatrix;"                       +
 "uniform mat4 u_pMatrix;"                       +
 "uniform int  toggleLight;"                   +
 "uniform vec3  u_la[2];"                        +
 "uniform vec3  u_ld[2];"                        +
 "uniform vec3  u_ls[2];"                        +
 "uniform vec4  u_lp[2];"                        +
 "uniform vec3  u_ka;"                           +
 "uniform vec3  u_kd;"                           +
 "uniform vec3  u_ks;"                           +
 "uniform float  u_kshiny;"                         +
 "out vec3 phong_ads_light;"                        +
 "vec3 light_direction[2];"                      +
 "vec3 reflection_vector[2];"                    +
 "vec3 intensity_ambient[2];"                    +
 "vec3 intensity_diffuse[2];"                    +
 "vec3 intensity_specular[2];"                   +
 "void main(void)"                                +
 "{"                                                +
 "    "                                            +
 "    if(toggleLight == 1)"                        +
 "    {    "                                        +
 "        vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;    "                            +
 "        vec3 t_norm = normalize( mat3( u_vMatrix * u_mMatrix) * vNormal );"                +
 "        vec3 view_vector = normalize( -eye_coord.xyz ); "                                +
 "        for( int i = 0 ; i < 2; i++ ) "                                                    +
 "        {    "                                                                            +
 "            light_direction[i] = normalize( vec3 ( u_lp[i] - eye_coord) );"                +
 "            reflection_vector[i] = reflect( -light_direction[i], t_norm );    "            +
 "            intensity_ambient[i] = u_la[i] * u_ka;    "                                    +
 "            intensity_diffuse[i] = u_ld[i] * u_kd * max( dot (light_direction[i], t_norm), 0.0f);    "                            +
 "            intensity_specular[i] = u_ls[i] * u_ks * pow( max( dot( reflection_vector[i], view_vector ), 0.0f), u_kshiny);    "    +
 "            phong_ads_light = phong_ads_light + intensity_ambient[i] + intensity_diffuse[i] + intensity_specular[i];"            +
 "        }        "                                +
 "    }        "                                    +
 "    else    "                                    +
 "    {    "                                        +
 "        phong_ads_light = vec3(1.0f);    "        +
 "    }    "                                        +
 "    gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition;"    +
 "}"
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(vertexShader);
    }
}
