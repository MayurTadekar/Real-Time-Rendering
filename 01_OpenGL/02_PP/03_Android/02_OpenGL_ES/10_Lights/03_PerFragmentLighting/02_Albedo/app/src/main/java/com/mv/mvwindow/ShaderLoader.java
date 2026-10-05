//
//  ShaderLoader.java
//  
//
//  Created by Mayur Tadekar on 18/11/25.
//
package com.mv.mvwindow;

import android.opengl.GLES32;

public class ShaderLoader
{
    private int shaderObject;
    
    private int[] iShaderCompiledStatus = new int[1];
    private int[] infoLogLength = new int[1];
    private String szInfoLog = null;
    
    public ShaderLoader(int type, String shaderName, String shaderSource)
    {
        
        // Code
        shaderObject = GLES32.glCreateShader(type);
        GLES32.glShaderSource(shaderObject, shaderSource);
        GLES32.glCompileShader(shaderObject);
        GLES32.glGetShaderiv(shaderObject,
                             GLES32.GL_COMPILE_STATUS,
                             iShaderCompiledStatus,
                             0);
        if(GLES32.GL_FALSE == iShaderCompiledStatus[0])
        {
            GLES32.glGetShaderiv(shaderObject,
                                GLES32.GL_INFO_LOG_LENGTH,
                                 infoLogLength,
                                 0);
            if( 0 < infoLogLength[0] )
            {
                szInfoLog = GLES32.glGetShaderInfoLog(shaderObject);
                System.out.println("MV: " + shaderName + " Compilation Error: " + szInfoLog);
                System.exit(0);
            }
        }
    }
    
    public int getShaderObject()
    {
        return(shaderObject);
    }
    
    public void deleteShader()
    {
        GLES32.glDeleteShader(shaderObject);
        shaderObject = 0;
    }
}
