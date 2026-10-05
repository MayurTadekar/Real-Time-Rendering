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
                                                             "#version 320 es\n"    +
                                                             "\n"                            +
                                                             "in vec4 vPosition;\n"            +
                                                             "in vec3 vNormal;\n"            +
                                                             "uniform mat4 u_modelViewMatrix;\n"                                               +
                                                             "uniform mat4 u_projectionMatrix;\n"                                              +
                                                             "uniform float toggleFlag;\n"                   +
                                                             "uniform vec3  u_ld;\n"                                                           +
                                                             "uniform vec3  u_kd;\n"                                                           +
                                                             "uniform vec4  u_LightPosition;\n"                                                +
                                                             "out vec3 diffuse_Light;\n"                                                       +
                                                             "void main(void)\n"                +
                                                             "{"                                                                             +
                                                             "  if( toggleFlag == 1.0f ) "                                                   +
                                                             "  {   "                                                                        +
                                                             "   vec4 eye_coord =  u_modelViewMatrix * vPosition;   "                        +
                                                             "   mat3 normal_matrix = mat3( transpose( inverse( u_modelViewMatrix ) ) );\n"    +
                                                             "   vec3 t_norm = normalize(normal_matrix * vNormal);\n"                          +
                                                             "   vec3 S = normalize( vec3(u_LightPosition - eye_coord) );\n"                   +
                                                             "   diffuse_Light = u_ld * u_kd * max( dot(S, t_norm), 0.0f)  ;\n"                +
                                                             "  }    "                                                                       +
                                                             "gl_Position = u_projectionMatrix *  u_modelViewMatrix * vPosition;\n"             +
                                                             "}"
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(vertexShader);
    }
}
