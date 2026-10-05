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
               "in vec3 phong_ads_light;"                        +
               "void main(void)"                                +
               "{"                                                +
               "    FragColor = vec4(phong_ads_light, 1.0f);"    +
               "}"
                                                               );
    
    public static String getShaderSource()
    {
        // Code
        return(fragmentShader);
    }
}
