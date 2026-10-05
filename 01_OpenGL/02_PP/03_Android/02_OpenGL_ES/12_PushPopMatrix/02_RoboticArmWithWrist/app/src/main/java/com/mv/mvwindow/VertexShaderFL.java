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
                                                             "uniform mediump int toggleFlag;"                                                +
                                                             "uniform vec3 u_la;"                                                        +
                                                             "uniform vec3 u_ld;"                                                        +
                                                             "uniform vec3 u_ls;"                                                        +
                                                             "uniform vec3 u_ka;"                                                        +
                                                             "uniform vec3 u_kd;"                                                        +
                                                             "uniform vec3 u_ks;"                                                        +
                                                             "uniform float u_kshine;"                                                    +
                                                             "uniform vec4 u_lightPosition;"                                                +
                                                             "out vec3 view_vector;"                                                        +
                                                             "out vec3 light_direction;"                                                    +
                                                             "out vec3 t_norm;"                                                            +
                                                             "void main(void)"                                                            +
                                                             "{"                                                                            +
                                                             "    if( toggleFlag == 1 )"                                                        +
                                                             "    {    "                                                                    +
                                                             "        vec4 eye_coord = u_vMatrix * u_mMatrix * vPosition;"                +
                                                             "        t_norm = normalize(mat3(u_vMatrix * u_mMatrix ) * vNormal);"    +
                                                             "        light_direction = normalize(vec3(u_lightPosition - eye_coord));"    +
                                                             "        view_vector = normalize(-eye_coord.xyz);"                            +
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
