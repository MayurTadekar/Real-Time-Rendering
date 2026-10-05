//
//  GLESView.java
//
//
//  Created by Mayur Tadekar on 17/11/25.
//
package com.mv.mvwindow;

import android.view.Gravity;
import android.graphics.Color;
import android.content.Context;

// OpenGL ES Related Packages
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;

// Events Related Packages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
    
    private Context context;
    private GestureDetector gestureDetector;
    private int shaderProgramObject;
    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int mvpMatrixUniform;
    private int vao[] = new int[1];
    private int vbo[] = new int[1];
    private float orthographicsProjectionMatrix[] = new float[16];
    
    public GLESView(Context _context)
    {
        // Code
        super(_context);

        context = _context;
        
        // Initialization of OpenGLES
        setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        
        // Get Gesture Detector
        gestureDetector = new GestureDetector(context, this, null, false);
        
        // Set this class as Double Tap Listener
        gestureDetector.setOnDoubleTapListener(this);
    }
    
    // 3 Implementable Methods of GLSurfaceView.Renderer
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        // Code
        int iResult = initialize(gl);
        
        if(0 != iResult)
        {
            System.out.println("MV: Initialize() Failed");
            System.exit(0);
        }
    }
    
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        // Code
        resize(width, height);
    }
    
    @Override
    public void onDrawFrame(GL10 gl)
    {
        // Code
        display();
        
        update();
    }
    
    // Implement 1st Method
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        // Code
        if(!gestureDetector.onTouchEvent(event))
        {
            super.onTouchEvent(event);
        }
        return(true);
    }
    
    // Implement 3 methods on Touch Events
    @Override
    public boolean onDoubleTap(MotionEvent event)
    {
        // Code

        return(true);
    }
    
    @Override
    public boolean onDoubleTapEvent(MotionEvent event)
    {
        // Code
        return(true);
    }
    
    @Override
    public boolean onSingleTapConfirmed(MotionEvent event)
    {
        // Code

        return(true);
    }
    
    @Override
    public boolean onDown(MotionEvent event)
    {
        // Code
        return(true);
    }
    
    @Override
    public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY)
    {
        // Code
        return(true);
    }
    
    @Override
    public void onLongPress(MotionEvent event)
    {
        // Code
    }
    
    @Override
    public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY)
    {
        // Code
        uninitialize();
        
        System.exit(0);

        return(true);
    }
    
    @Override
    public void onShowPress(MotionEvent event)
    {
        // Code
    }
    
    @Override
    public boolean onSingleTapUp(MotionEvent event)
    {
        // Code
        return(true);
    }
    
    // OUR CUSTOM OpenGL METHODS
    
    private int initialize(GL10 gl)
    {
        // Code
        
        printGLESInfo(gl);
        
        // Vertex Shader
        final String vertexShaderSourceCode = String.format(
            "#version 320 es\n"  +
            "in vec4 vPosition;"                +
            "uniform mat4 u_mvpMatrix;"         +
            "void main(void)\n"  +
            "{\n"                +
            "   gl_Position = u_mvpMatrix * vPosition;"                +
            "}\n"                +
            "\n"
        );
        
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
        GLES32.glCompileShader(vertexShaderObject);
        
        int shaderCompileStatus[] = new int[1];
        int infoLogLength[] = new int[1];
        String szInfoLog = null;
        
        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
        if( GLES32.GL_FALSE == shaderCompileStatus[0] )
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if( 0 < infoLogLength[0])
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("MV: Vertex Shader Compilation Log = " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        
        // Fagment Shader
        final String fragmentShaderSourceCode = String.format(
            "#version 320 es\n"  +
            "precision highp float;\n"  +
            "out vec4 FragColor;\n"                +
            "void main(void)\n"  +
            "{\n"                +
            "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"                +
            "}\n"                +
            "\n"
        );
        
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
        GLES32.glCompileShader(fragmentShaderObject);
        
        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
        if( GLES32.GL_FALSE == shaderCompileStatus[0] )
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if( 0 < infoLogLength[0])
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("MV: Fragment Shader Compilation Log = " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
            
        // Shader Program
        shaderProgramObject = GLES32.glCreateProgram();
        
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
        
        GLES32.glBindAttribLocation(shaderProgramObject, MyAttributes.MV_ATTRIBUTE_POSITION, "vPosision");
        
        GLES32.glLinkProgram(shaderProgramObject);
        
        int iShaderProgramLinkStatus[] = new int[1];
        infoLogLength[0] = 0;
        szInfoLog = null;
        
        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if(GLES32.GL_FALSE == iShaderProgramLinkStatus[0])
        {
            GLES32.glGetProgramiv(  shaderProgramObject,
                                    GLES32.GL_INFO_LOG_LENGTH,
                                    infoLogLength,
                                    0);
            if( 0 < infoLogLength[0])
            {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("MV: Shader Program Link Log = " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        
        mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
        
        // Vertices, Color, Shader Attribs, VAO, VBO.
        
        final float triangleVertices[]  = new float[] {
            0.0f, 50.0f, 0.0f,
            -50.0f, -50.0f, 0.0f,
            50.0f, -50.0f, 0.0f
        };
        
        GLES32.glGenVertexArrays(1, vao, 0);
        
        GLES32.glBindVertexArray(vao[0]);
        {
            GLES32.glGenBuffers(1, vbo, 0);
            
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);
            
            ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
            byteBuffer.order(ByteOrder.nativeOrder());
            
            FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
            
            verticesBuffer.put(triangleVertices);
            verticesBuffer.position(0);
            
            GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                                triangleVertices.length * 4,
                                verticesBuffer,
                                GLES32.GL_STATIC_DRAW);
            
            GLES32.glVertexAttribPointer(MyAttributes.MV_ATTRIBUTE_POSITION,
                                         3,
                                         GLES32.GL_FLOAT,
                                         false,
                                         0,
                                         0);
            GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_POSITION);
            
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        }
        GLES32.glBindVertexArray(0);
        
        // Depth Initialization
        GLES32.glClearDepthf(1.0f);
        
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        
        // Set the Clear Color
        GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        
        Matrix.setIdentityM(orthographicsProjectionMatrix, 0);
        
        return(0);
    }
    
    private void printGLESInfo(GL10 gl)
    {
        // Code
        
        String gles_vendor = gl.glGetString(GL10.GL_VENDOR);
        String gles_renderer = gl.glGetString(GL10.GL_RENDERER);
        String gles_version = gl.glGetString(GL10.GL_VERSION);
        
        System.out.println("MV: Vendor = " + gles_vendor);
        System.out.println("MV: Renderer = " + gles_renderer);
        System.out.println("MV: Version = " + gles_version);
    }
    
    private void resize(int width, int height)
    {
        // Code
        if(height < 0)
        {
            height = 1;
        }
        
        GLES32.glViewport(0, 0, width, height);
        
        if( width <= height )
        {
            Matrix.orthoM(orthographicsProjectionMatrix,
                          0,
                          -100.0f,
                          100.0f,
                          (-100.0f * (height/width)),
                          (100.0f * (height/width)),
                          -100.0f,
                          100.0f);
        }
        else
        {
            Matrix.orthoM(orthographicsProjectionMatrix,
                          0,
                          (-100.0f * (width/height)),
                          (100.0f * (width/height)),
                          -100.0f,
                          100.0f,
                          -100.0f,
                          100.0f);
        }
        
    }
    
    private void display()
    {
        // Code
        
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        
        GLES32.glUseProgram(shaderProgramObject);
        
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];
        
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
                          orthographicsProjectionMatrix, 0,
                          modelViewMatrix, 0);
        
        GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);
        
        GLES32.glBindVertexArray(vao[0]);
        
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);
        
        GLES32.glBindVertexArray(0);
        
        GLES32.glUseProgram(0);
        
        requestRender();
    }
    
    private void update()
    {
        // Code
    }
    
    private void uninitialize()
    {
        // Code
        if( 0 != vao[0] )
        {
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }
        
        if( 0 != vbo[0] )
        {
            GLES32.glDeleteVertexArrays(1, vbo, 0);
            vbo[0] = 0;
        }
        
        if( 0 != shaderProgramObject )
        {
            if( 0 != vertexShaderObject )
            {
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }
            
            if( 0 != fragmentShaderObject )
            {
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }
        
        if( 0 != shaderProgramObject )
        {
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
    }
}
