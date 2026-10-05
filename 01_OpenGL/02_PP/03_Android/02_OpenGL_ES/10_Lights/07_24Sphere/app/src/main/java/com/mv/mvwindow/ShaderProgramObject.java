//
//  ShaderProgramObject.java
//  
//
//  Created by Mayur Tadekar on 18/11/25.
//

package com.mv.mvwindow;

import android.opengl.GLES32;

public class ShaderProgramObject
{
    
    private int shaderProgramObject;
    private int vertexShader;
    private int fragmentShader;
    
    public ShaderProgramObject(int vertexShader, int fragmentShader)
    {
        // Code
        shaderProgramObject = GLES32.glCreateProgram();
        
        System.out.println("MV: Shader Program Create");
        
        GLES32.glAttachShader(shaderProgramObject, vertexShader);
        GLES32.glAttachShader(shaderProgramObject, fragmentShader);
        
        System.out.println("MV: Shaders Attached");
    }
    
    public void linkObject()
    {
        // Code
        GLES32.glLinkProgram(shaderProgramObject);
        
        System.out.println("MV: Shader Linked");
        
        int iShaderProgramLinkStatus[] = new int[1];
        int infoLength[] = new int[1];
        infoLength[0] = 0;
        String szInfoLog = null;
        
        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if( GLES32.GL_FALSE == iShaderProgramLinkStatus[0])
        {
            GLES32.glGetProgramiv(shaderProgramObject,
                                  GLES32.GL_INFO_LOG_LENGTH,
                                  infoLength,
                                  0);
            if( 0 < infoLength[0] )
            {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("MV: Shader Program Link Log = " + szInfoLog);
                System.exit(0);
            }
        }
        
        System.out.println("MV: Shader Linking Completed");
    }
    
    public int getObject()
    {
        // Code
        return(shaderProgramObject);
    }
    
    public void deleteObject()
    {
        GLES32.glDetachShader(shaderProgramObject, vertexShader);
        GLES32.glDetachShader(shaderProgramObject, fragmentShader);
        GLES32.glDeleteShader(shaderProgramObject);
        shaderProgramObject = 0;
    }
}
