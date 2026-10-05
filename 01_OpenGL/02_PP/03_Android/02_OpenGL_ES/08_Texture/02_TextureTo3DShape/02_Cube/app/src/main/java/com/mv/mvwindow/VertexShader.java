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
                                                             "\n" +
                                                             "in vec4 vPosition;\n" +
                                                             "in vec2 vTexCoord;\n" +
                                                             "out vec2 out_vTexCoord;\n" +
                                                             "uniform mat4 u_mvpMatrix;\n" +
                                                             "void main(void)\n" +
                                                             "{\n" +
                                                             "  gl_Position = u_mvpMatrix * vPosition;\n" +
                                                             "  out_vTexCoord = vTexCoord;\n" +
                                                             "}\n" +
                                                             "\n"
                                                             );
    
    public static String getShaderSource()
    {
        // Code
        return(vertexShader);
    }
}
