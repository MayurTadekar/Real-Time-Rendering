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
import java.nio.ShortBuffer;

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
    
    private Context context;
    private GestureDetector gestureDetector;
    private ShaderProgramObject shaderProgramObject;
    private ShaderLoader vertexShaderObject;
    private ShaderLoader fragmentShaderObject;
    private int mMatrixUniform;
    private int vMatrixUniform;
    private int pMatrixUniform;
    private float perspectiveProjectionMatrix[] = new float[16];
    
    private float angle = 0.0f;
    private float lightAngle = 0.0f;
    
    float sphere_vertices[] = new float[1146];
    float sphere_normals[] = new float[1146];
    float sphere_textures[] = new float[764];
    short sphere_elements[] = new short[2280];
    
    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
    
    private int numVertices;
    private int numElements;
    
    private boolean toggleAnimation = false;
    private int toggleLightFlag = 0;
    private int uniformLightToggleFlag = 0;
    
    private int uniformLightAmbient1 = 0;
    private int uniformLightAmbient2 = 0;
    private int uniformLightAmbient3 = 0;
    private int uniformLightDiffuse1 = 0;
    private int uniformLightDiffuse2 = 0;
    private int uniformLightDiffuse3 = 0;
    private int uniformLightSpecular1 = 0;
    private int uniformLightSpecular2 = 0;
    private int uniformLightSpecular3 = 0;
    private int uniformLightPosition1 = 0;
    private int uniformLightPosition2 = 0;
    private int uniformLightPosition3 = 0;
    
    private int uniformMaterialAmbient = 0;
    private int uniformMaterialDiffuse = 0;
    private int uniformMaterialSpecular = 0;
    private int uniformMaterialShininess = 0;
    
    
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
        
        if(toggleAnimation)
        {
            toggleAnimation = false;
        }
        else
        {
            toggleAnimation = true;
        }
        
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
        toggleLightFlag++;
        
        if(toggleLightFlag > 2)
            toggleLightFlag = 0;
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
        vertexShaderObject = new ShaderLoader(GLES32.GL_VERTEX_SHADER,
                                              "Vertex Shader",
                                              VertexShader.getShaderSource());
        
        // Fagment Shader
        fragmentShaderObject = new ShaderLoader(GLES32.GL_FRAGMENT_SHADER,
                                                "Fragment Shader",
                                                FragmentShader.getShaderSource());
        
            
        // Shader Program
        
        shaderProgramObject = new ShaderProgramObject(
                                                      vertexShaderObject.getShaderObject(),
                                                      fragmentShaderObject.getShaderObject()
                                                      );
        
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_COLOR, "vColor");
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_NORMAL, "vNormal");
        
        shaderProgramObject.linkObject();
        
        mMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_mMatrix");
        vMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_vMatrix");
        pMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_pMatrix");
        
        uniformLightToggleFlag = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "toggleFlag");
        
        uniformLightAmbient1 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_la[0]");
        uniformLightAmbient2 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_la[1]");
        uniformLightAmbient3 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_la[2]");
        uniformLightDiffuse1 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ld[0]");
        uniformLightDiffuse2 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ld[1]");
        uniformLightDiffuse3 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ld[2]");
        uniformLightSpecular1 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ls[0]");
        uniformLightSpecular2 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ls[1]");
        uniformLightSpecular3 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ls[2]");
        uniformLightPosition1 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_lightPosition[0]");
        uniformLightPosition2 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_lightPosition[1]");
        uniformLightPosition3 = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_lightPosition[2]");
        
        uniformMaterialAmbient = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ka");
        uniformMaterialDiffuse = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_kd");
        uniformMaterialSpecular = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ks");
        uniformMaterialShininess = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_kshine");
        
        Sphere sphere = new Sphere();
        
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();
        
        // Vertices, Color, Shader Attribs, vaoRectangle, vboRectangleVertices.
        
        GLES32.glGenVertexArrays(1, vao_sphere, 0);
        
        GLES32.glBindVertexArray(vao_sphere[0]);
        {
            // Position
            GLES32.glGenBuffers(1, vbo_sphere_position, 0);
            {
                GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);
                
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
                byteBuffer.order(ByteOrder.nativeOrder());
                
                FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
                
                verticesBuffer.put(sphere_vertices);
                verticesBuffer.position(0);
                
                GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                                    sphere_vertices.length * 4,
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
            
            // normal
            GLES32.glGenBuffers(1, vbo_sphere_normal, 0);
            {
                GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);
                
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
                byteBuffer.order(ByteOrder.nativeOrder());
                
                FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
                
                verticesBuffer.put(sphere_normals);
                verticesBuffer.position(0);
                
                GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                                    sphere_normals.length * 4,
                                    verticesBuffer,
                                    GLES32.GL_STATIC_DRAW);
                
                GLES32.glVertexAttribPointer(MyAttributes.MV_ATTRIBUTE_NORMAL,
                                             3,
                                             GLES32.GL_FLOAT,
                                             false,
                                             0,
                                             0);
                GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_NORMAL);
            }
            GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
            
            GLES32.glGenBuffers(1, vbo_sphere_element, 0);
            {
                GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
                
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
                byteBuffer.order(ByteOrder.nativeOrder());
                
                ShortBuffer elementsBuffer = byteBuffer.asShortBuffer();
                elementsBuffer.put(sphere_elements);
                elementsBuffer.position(0);
                
                GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                                    sphere_elements.length * 2,
                                    elementsBuffer,
                                    GLES32.GL_STATIC_DRAW);
            }
            GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        GLES32.glBindVertexArray(0);
        
        
        // Depth Initialization
        GLES32.glClearDepthf(1.0f);
        
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        
        // Set the Clear Color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
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
        
        GLES32.glViewport(0, 0, width, height);
        
        float ratio = (float)width / (float)height;
        
        Matrix.perspectiveM(perspectiveProjectionMatrix, 0,
                            45.0f, ratio, 0.1f, 100.0f);
    }
    
    private void display()
    {
        // Code
        
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        
        
        float translateMatrix[] = new float[16];
        float rotateMatrix[] = new float[16];
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];

        float lightAmbient1[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
        float lightAmbient2[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
        float lightAmbient3[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
        float lightDefuse1[] = new float[]{1.0f, 0.0f, 0.0f, 1.0f};
        float lightDefuse2[] = new float[]{0.0f, 1.0f, 0.0f, 1.0f};
        float lightDefuse3[] = new float[]{0.0f, 0.0f, 1.0f, 1.0f};
        float lightSpecular1[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float lightSpecular2[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float lightSpecular3[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        
        float lightPosition1[] = new float[]{(float)Math.sin(Math.toRadians(lightAngle)), 0.0f, (float)Math.cos(Math.toRadians(lightAngle)), 1.0f};
//        float lightPosition2[] = new float[]{0.0f, 0.0f, -5.0f, 1.0f};
//        float lightPosition3[] = new float[]{0.0f, 0.0f, -5.0f, 1.0f};
        float lightPosition2[] = new float[]{0.0f, (float)Math.sin(Math.toRadians(lightAngle)), (float)Math.cos(Math.toRadians(lightAngle)), 1.0f};
        float lightPosition3[] = new float[]{(float)Math.sin(Math.toRadians(lightAngle)), (float)Math.cos(Math.toRadians(lightAngle)), 0.0f, 1.0f};

        float materialAmbient[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
        float materialDefuse[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float materialSpecular[] = new float[]{0.7f, 0.7f, 0.7f, 1.0f};
        float materialShiness = 128.0f;
        
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(translateMatrix, 0);
        Matrix.setIdentityM(rotateMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        Matrix.setLookAtM(viewMatrix, 0,
                         0.0f, 0.0f, 2.0f,
                         0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f);
        
        Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, 0.0f);
        
        Matrix.rotateM(rotateMatrix, 0, angle, 1.0f, 1.0f, 1.0f);	
        
        Matrix.multiplyMM(modelMatrix, 0,
                          modelMatrix, 0,
                          translateMatrix, 0);
        // Matrix.multiplyMM(modelMatrix, 0,
        //                   modelMatrix, 0,
        //                   rotateMatrix, 0);

        Matrix.multiplyMM(modelViewMatrix, 0,
                          viewMatrix, 0,
                          modelMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
                          perspectiveProjectionMatrix, 0,
                          modelViewMatrix, 0);


        GLES32.glUseProgram(shaderProgramObject.getObject());

        GLES32.glUniformMatrix4fv(mMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(vMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(pMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

        if( toggleLightFlag > 0 )
        {
            GLES32.glUniform1i(uniformLightToggleFlag, toggleLightFlag);
            
            GLES32.glUniform3fv( uniformLightAmbient1, 1, lightAmbient1, 0 );
            GLES32.glUniform3fv( uniformLightAmbient2, 1, lightAmbient2, 0 );
            GLES32.glUniform3fv( uniformLightAmbient3, 1, lightAmbient3, 0 );
            GLES32.glUniform3fv( uniformLightDiffuse1, 1, lightDefuse1, 0 );
            GLES32.glUniform3fv( uniformLightDiffuse2, 1, lightDefuse2, 0 );
            GLES32.glUniform3fv( uniformLightDiffuse3, 1, lightDefuse3, 0 );
            GLES32.glUniform3fv( uniformLightSpecular1, 1, lightSpecular1, 0 );
            GLES32.glUniform3fv( uniformLightSpecular2, 1, lightSpecular2, 0 );
            GLES32.glUniform3fv( uniformLightSpecular3, 1, lightSpecular3, 0 );
            GLES32.glUniform4fv( uniformLightPosition1, 1, lightPosition1, 0 );
            GLES32.glUniform4fv( uniformLightPosition2, 1, lightPosition2, 0 );
            GLES32.glUniform4fv( uniformLightPosition3, 1, lightPosition3, 0 );

            GLES32.glUniform3fv( uniformMaterialAmbient, 1, materialAmbient, 0 );
            GLES32.glUniform3fv( uniformMaterialDiffuse, 1, materialDefuse, 0 );
            GLES32.glUniform3fv( uniformMaterialSpecular, 1, materialSpecular, 0 );
            GLES32.glUniform1f( uniformMaterialShininess, materialShiness );
        }
        else
        {
            GLES32.glUniform1i(uniformLightToggleFlag, 0);
        }

        GLES32.glBindVertexArray(vao_sphere[0]);
        {
            GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
            GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        }
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);
        
        requestRender();
    }
    
    private void update()
    {
        // Code
        
        if(toggleAnimation)
        {
            angle += 0.5f;
            lightAngle += 0.5f;
        }
        
        if(angle > 360.0f)
        {
            angle -= 360.0f;
        }
        
        if(lightAngle > 360.0f)
        {
            lightAngle -= 360.0f;
        }
    }
    
    private void uninitialize()
    {
        // Code
        if( 0 != vao_sphere[0] )
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0] = 0;
        }
        
        if( 0 != vbo_sphere_position[0] )
        {
            GLES32.glDeleteVertexArrays(1, vbo_sphere_position, 0);
            vbo_sphere_position[0] = 0;
        }
        
        if( 0 != vbo_sphere_normal[0] )
        {
            GLES32.glDeleteVertexArrays(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0] = 0;
        }
        
        if( 0 != vbo_sphere_element[0] )
        {
            GLES32.glDeleteVertexArrays(1, vbo_sphere_element, 0);
            vbo_sphere_element[0] = 0;
        }
        
        if( 0 != shaderProgramObject.getObject() )
        {
            shaderProgramObject.deleteObject();
        }
        
        if( 0 != vertexShaderObject.getShaderObject() )
        {
            vertexShaderObject.deleteShader();
        }
        
        if( 0 != fragmentShaderObject.getShaderObject() )
        {
            fragmentShaderObject.deleteShader();
        }
    }
}
