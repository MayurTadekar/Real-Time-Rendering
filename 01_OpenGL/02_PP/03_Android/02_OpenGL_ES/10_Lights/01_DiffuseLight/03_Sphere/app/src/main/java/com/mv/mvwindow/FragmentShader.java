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
                                                             "#version 320 es\n"    +
                                                               "\n"                            +
                                                               "precision highp float;\n" +
                                                               "vec4 color;\n"                                   +
                                                               "out vec4 Fragcolor;\n"                           +
                                                               "in  vec4 out_Color;\n"                           +
                                                               "in  vec3 diffuse_Light;\n"                       +
                                                               "uniform float toggleFlag;\n"                   +
                                                               "void main(void)"                               +
                                                               "{"                                             +
                                                               "   if( toggleFlag == 1.0f)"                    +
                                                               "   {"                                          +
                                                               "       color = vec4( diffuse_Light, 1.0f );\n"   +
                                                               "   } else {"                                   +
                                                               "       color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"  +
                                                               "   }"                                          +
                                                               "   Fragcolor = color;\n"                         +
                                                               "}"
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
