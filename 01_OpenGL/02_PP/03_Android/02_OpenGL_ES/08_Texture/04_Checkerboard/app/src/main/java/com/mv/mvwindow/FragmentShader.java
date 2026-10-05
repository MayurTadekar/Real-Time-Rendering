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
                                                             "\n" +
                                                             "precision highp float;\n" +
                                                             "out vec4 FragColor;\n" +
                                                             "in vec2 out_vTexCoord;\n" +
                                                             "uniform highp sampler2D u_Sampler2D;\n" +
                                                             "void main(void)\n" +
                                                             "{\n" +
                                                             "  FragColor = texture(u_Sampler2D, out_vTexCoord);\n" +
                                                             "}\n" +
                                                             "\n" 
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
