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
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

// OpenGL ES Related Packages
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;
import android.opengl.GLUtils;

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
    private ShaderProgramObject shaderProgramObject;
    private ShaderLoader vertexShader;
    private ShaderLoader fragmentShader;
    private int mvpMatrixUniform;
    private int vao[] = new int[1];
    private int vbo[] = new int[1];
    private int vboTexture[] = new int[1];
    private float perspectiveProjectionMatrix[] = new float[16];
    
    private int[] textureChecker = new int[1];
    private int uSampler;
    private byte checkImage[] = new byte[64 * 64 * 4];
    
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
    
    private void createCheckerImage()
    {
        // Code
        byte c;
        
        for(int ole = 0; ole < 64; ++ole)
        {
            for(int ile = 0; ile < 64; ++ile)
            {
                c = (byte)(((ole & 8) ^ (ile & 8)) * 255);
                
                checkImage[(ole * 64 + ile) * 4 + 0] = (byte)c;
                checkImage[(ole * 64 + ile) * 4 + 1] = (byte)c;
                checkImage[(ole * 64 + ile) * 4 + 2] = (byte)c;
                checkImage[(ole * 64 + ile) * 4 + 3] = (byte)0xFF;
            }
        }
    }
    
    private int glLoadImage()
    {
        // Code
        createCheckerImage();
        
        Bitmap bitmap = Bitmap.createBitmap(64, 64, Bitmap.Config.ARGB_8888);
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(64 * 64 * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
        byteBuffer.put(checkImage);
        byteBuffer.position(0);
        
        bitmap.copyPixelsFromBuffer(byteBuffer);
        
        int texture[] = new int[1];
        
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
        GLES32.glGenTextures(1, textureChecker, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
        {
            GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
            GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
            GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
            GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);
        }
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
        
        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D,
                           0,
                           bitmap,
                           0);
        
        return(texture[0]);
    }
    
    private int initialize(GL10 gl)
    {
        // Code
        
        printGLESInfo(gl);
        
        // Vertex Shader
        vertexShader = new ShaderLoader(GLES32.GL_VERTEX_SHADER,
                                              "Vertex Shader",
                                              VertexShader.getShaderSource());
        
        // Fagment Shader
        fragmentShader = new ShaderLoader(GLES32.GL_FRAGMENT_SHADER,
                                                "Fragment Shader",
                                                FragmentShader.getShaderSource());
        
            
        // Shader Program
        
        shaderProgramObject = new ShaderProgramObject(
                                                      vertexShader.getShaderObject(),
                                                      fragmentShader.getShaderObject()
                                                      );
        
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_TEXCOORD, "vTexCoord");
        
        shaderProgramObject.linkObject();
        
        mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_mvpMatrix");
        uSampler = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_Sampler");
        
        // Vertices, Color, Shader Attribs, VAO, VBO.
        
        final float rectangleVertices[]  = new float[] {
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
        };
        
        final float rectangleTexcoord[]  = new float[] {
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
        };
        
        
        GLES32.glGenVertexArrays(1, vao, 0);
        
        GLES32.glBindVertexArray(vao[0]);
        {
            GLES32.glGenBuffers(1, vbo, 0);
            
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);
            {
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rectangleVertices.length * 4);
                byteBuffer.order(ByteOrder.nativeOrder());
                
                FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
                
                verticesBuffer.put(rectangleVertices);
                verticesBuffer.position(0);
                
                GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                                    rectangleVertices.length * 4,
                                    verticesBuffer,
                                    GLES32.GL_STATIC_DRAW);
                
                GLES32.glVertexAttribPointer(MyAttributes.MV_ATTRIBUTE_POSITION,
                                             3,
                                             GLES32.GL_FLOAT,
                                             false,
                                             0,
                                             0);
                GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_POSITION);
            }
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
            
            GLES32.glGenBuffers(1, vboTexture, 0);
            
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboTexture[0]);
            {
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rectangleTexcoord.length * 4);
                byteBuffer.order(ByteOrder.nativeOrder());
                
                FloatBuffer texcoordBuffer = byteBuffer.asFloatBuffer();
                
                texcoordBuffer.put(rectangleTexcoord);
                texcoordBuffer.position(0);
                
                GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                                    rectangleTexcoord.length * 4,
                                    texcoordBuffer,
                                    GLES32.GL_STATIC_DRAW);
                
                GLES32.glVertexAttribPointer(MyAttributes.MV_ATTRIBUTE_TEXCOORD,
                                             2,
                                             GLES32.GL_FLOAT,
                                             false,
                                             0,
                                             0);
                GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_TEXCOORD);
            }
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        }
        GLES32.glBindVertexArray(0);
        
        // Texture
        textureChecker[0] = glLoadImage();
        
        // Depth Initialization
        GLES32.glClearDepthf(1.0f);
        
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        
        // Set the Clear Color
        GLES32.glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        
        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
        
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
        
        float ratio = (float)width / (float)height;
        
        Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, ratio, 0.1f, 100.0f);
    }
    
    private void display()
    {
        // Code
        
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        
        float modelMatrix[] = new float[16];
        float rotateMatrix[] = new float[16];
        float viewMatrix[] = new float[16];
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];
        
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        Matrix.setLookAtM(viewMatrix, 0,
                         0.0f, 0.0f, 5.0f,
                         0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f);
        
        Matrix.translateM(modelMatrix, 0, -1.5f, 0.0f, 0.0f);
        
        Matrix.multiplyMM(modelViewMatrix, 0,
                          viewMatrix, 0,
                          modelMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
                 perspectiveProjectionMatrix, 0,
                 modelViewMatrix, 0);
        
        GLES32.glUseProgram(shaderProgramObject.getObject());
        
        GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);
        
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureChecker[0]);
        GLES32.glUniform1i(uSampler, 0);
        
        GLES32.glBindVertexArray(vao[0]);
        
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 6);
        
        GLES32.glBindVertexArray(0);
        
        GLES32.glUseProgram(0);
        
        
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(rotateMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        Matrix.translateM(modelMatrix, 0, 1.5f, 0.0f, -0.5f);
        Matrix.rotateM(rotateMatrix, 0, 30.0f, 0.0f, 1.0f, 0.0f);
        
        Matrix.multiplyMM(modelMatrix, 0,
                          modelMatrix, 0,
                          rotateMatrix, 0);
        
        Matrix.multiplyMM(modelViewMatrix, 0,
                          viewMatrix, 0,
                          modelMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
                 perspectiveProjectionMatrix, 0,
                 modelViewMatrix, 0);
        
        GLES32.glUseProgram(shaderProgramObject.getObject());
        
        GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);
        
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureChecker[0]);
        GLES32.glUniform1i(uSampler, 0);
        
        GLES32.glBindVertexArray(vao[0]);
        
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 6);
        
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
        
        if( 0 != vboTexture[0] )
        {
            GLES32.glDeleteVertexArrays(1, vboTexture, 0);
            vbo[0] = 0;
        }
        
        if( shaderProgramObject.getObject() != 0 )
        {
            shaderProgramObject.deleteObject();
        }

        if( vertexShader.getShaderObject() != 0)
        {
            vertexShader.deleteShader();
        }

        if( fragmentShader.getShaderObject() != 0)
        {
            fragmentShader.deleteShader();
        }
    }
}
