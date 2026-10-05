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
                                                             "in vec4 out_vColor;\n" +
                                                             "void main(void)\n" +
                                                             "{\n" +
                                                             "  FragColor = out_vColor;\n" +
                                                             "}\n" +
                                                             "\n" 
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
