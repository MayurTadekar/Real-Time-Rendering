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
    
    private int uniformLightAmbient = 0;
    private int uniformLightDiffuse = 0;
    private int uniformLightSpecular = 0;
    
    private int uniformMaterialAmbient = 0;
    private int uniformMaterialDiffuse = 0;
    private int uniformMaterialSpecular = 0;
    private int uniformMaterialShininess = 0;
    
    private int uniformPosition = 0;
    
    private int lightAxis = 1;
    
    float[][] materialAmbient = new float[][]{    {0.0215f,0.1745f,0.0215f,1.0f},
                                                    {0.135f,0.2225f,0.1575f,1.0f},
                                                    {0.05375f,0.05f,0.06625f,1.0f},
                                                    {0.25f,0.20725f,0.20725f,1.0f},
                                                    {0.1745f,0.01175f,0.01175f,1.0f},
                                                    {0.1f,0.18725f,0.1745f,1.0f},
                                                    {0.329412f,0.223529f,0.027451f,1.0f},
                                                    {0.2125f,0.1275f,0.054f,1.0f},
                                                    {0.25f,0.25f,0.25f,1.0f},
                                                    {0.19125f,0.0735f,0.0225f,1.0f},
                                                    {0.24725f,0.1995f,0.0745f,1.0f},
                                                    {0.19225f,0.19225f,0.19225f,1.0f},
                                                    {0.0f,0.0f,0.0f,1.0f},
                                                    {0.0f,0.1f,0.06f,1.0f},
                                                    {0.0f,0.0f,0.0f,1.0f},
                                                    {0.0f,0.0f,0.0f,1.0f},
                                                    {0.0f,0.0f,0.0f,1.0f},
                                                    {0.0f,0.0f,0.0f,1.0f},
                                                    {0.02f,0.02f,0.02f,1.0f},
                                                    {0.0f,0.05f,0.05f,1.0f},
                                                    {0.0f,0.05f,0.0f,1.0f},
                                                    {0.05f,0.0f,0.0f,1.0f},
                                                    {0.05f,0.05f,0.05f,1.0f},
                                                    {0.05f,0.05f,0.0f,1.0f}
                                    };

        float[][] materialDefuse = new float[][]{     {0.07568f,0.61424f,0.07568f,1.0f},
                                                    {0.54f,0.89f,0.63f,1.0f},
                                                    {0.18275f,0.17f,0.22525f,1.0f},
                                                    {1.0f,0.829f,0.829f,1.0f},
                                                    {0.61424f,0.04136f,0.04136f,1.0f},
                                                    {0.396f,0.74151f,0.69102f,1.0f},
                                                    {0.780392f,0.568627f,0.113725f,1.0f},
                                                    {0.714f,0.4284f,0.18144f,1.0f},
                                                    {0.4f,0.4f,0.4f,1.0f},
                                                    {0.7038f,0.27048f,0.0828f,1.0f},
                                                    {0.75164f,0.60648f,0.22648f,1.0f},
                                                    {0.50754f,0.50754f,0.50754f,1.0f},
                                                    {0.01f,0.01f,0.01f,1.0f},
                                                    {0.0f,0.50980392f,0.50980392f,1.0f},
                                                    {0.1f,0.35f,0.1f,1.0f},
                                                    {0.5f,0.0f,0.0f,1.0f},
                                                    {0.55f,0.55f,0.55f,1.0f},
                                                    {0.5f,0.5f,0.0f,1.0f},
                                                    {0.01f,0.01f,0.01f,1.0f},
                                                    {0.4f,0.5f,0.5f,1.0f},
                                                    {0.4f,0.5f,0.4f,1.0f},
                                                    {0.5f,0.4f,0.4f,1.0f},
                                                    {0.5f,0.5f,0.5f,1.0f},
                                                    {0.5f,0.5f,0.4f,1.0f}
                                                    };

        float[][] materialSpecular = new float[][] {    {0.633f,0.727811f,0.633f,1.0f},
                                                        {0.316228f,0.316228f,0.316228f,1.0f},
                                                        {0.332741f,0.328634f,0.346435f,1.0f},
                                                        {0.296648f,0.296648f,0.296648f,1.0f},
                                                        {0.727811f,0.626959f,0.626959f,1.0f},
                                                        {0.297254f,0.30829f,0.306678f,1.0f},
                                                        {0.992157f,0.941176f,0.807843f,1.0f},
                                                        {0.393548f,0.271906f,0.166721f,1.0f},
                                                        {0.774597f,0.774597f,0.774597f,1.0f},
                                                        {0.256777f,0.137622f,0.086014f,1.0f},
                                                        {0.628281f,0.555802f,0.366065f,1.0f},
                                                        {0.508273f,0.508273f,0.508273f,1.0f},
                                                        {0.50f,0.50f,0.50f,1.0f},
                                                        {0.50196078f,0.50196078f,0.50196078f,1.0f},
                                                        {0.45f,0.55f,0.45f,1.0f},
                                                        {0.7f,0.6f,0.6f,1.0f},
                                                        {0.70f,0.70f,0.70f,1.0f},
                                                        {0.60f,0.60f,0.50f,1.0f},
                                                        {0.4f,0.4f,0.4f,1.0f},
                                                        {0.04f,0.7f,0.7f,1.0f},
                                                        {0.04f,0.7f,0.04f,1.0f},
                                                        {0.7f,0.04f,0.04f,1.0f},
                                                        {0.7f,0.7f,0.7f,1.0f},
                                                        {0.7f,0.7f,0.04f,1.0f}
                                                };
        float[] materialShiness = new float[] {    0.6f * 128,
                                    0.1f * 128,
                                    0.3f * 128,
                                    0.088f * 128,
                                    0.6f * 128,
                                    0.1f * 128,
                                    0.21794872f * 128,
                                    0.2f * 128,
                                    0.6f * 128,
                                    0.1f * 128,
                                    0.4f * 128,
                                    0.4f * 128,
                                    0.25f * 128,
                                    0.25f * 128,
                                    0.25f * 128,
                                    0.25f * 128,
                                    0.25f * 128,
                                    0.25f * 128,
                                    0.078125f * 128,
                                    0.078125f * 128,
                                    0.078125f * 128,
                                    0.078125f * 128,
                                    0.078125f * 128,
                                    0.078125f * 128
                                };
    
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
        toggleLightFlag++;
        
        if(toggleLightFlag > 2)
            toggleLightFlag = 0;
        
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
        lightAxis++;
        
        if(lightAxis > 3)
            lightAxis = 1;
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
        
        uniformLightAmbient = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_la");
        uniformLightDiffuse = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ld");
        uniformLightSpecular = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ls");
        
        uniformMaterialAmbient = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ka");
        uniformMaterialDiffuse = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_kd");
        uniformMaterialSpecular = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ks");
        uniformMaterialShininess = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_kshine");
    
        uniformPosition = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_lightPosition");
        
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
        
        GLES32.glViewport(0, 0, width, height);
        
        float ratio = (float)width / (float)height;
        
        Matrix.perspectiveM(perspectiveProjectionMatrix, 0,
                            30.0f, ratio, 0.1f, 100.0f);
    }
    
    private void display()
    {
        // Code
        
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        
        
        float translateMatrix[] = new float[16];
        float rotateMatrix[] = new float[16];
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];

        float lightAmbient[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
        float lightDefuse[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float lightSpecular[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};

        float lightPosition[] = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
        
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(translateMatrix, 0);
        Matrix.setIdentityM(rotateMatrix, 0);
        
        Matrix.setLookAtM(viewMatrix, 0,
                          1.5f * 2.5f, 1.5f * 1.5f, 12.0f,
                          1.5f * 2.5f, 1.5f * 1.5f, 0.0f,
                          0.0f, 1.0f, 0.0f);

        GLES32.glUseProgram(shaderProgramObject.getObject());

        
        if( 1 == lightAxis )
        {
            lightPosition[0] = 5.0f * (float)Math.sin(Math.toRadians(angle));
            lightPosition[1] = 0.0f;
            lightPosition[2] = 5.0f * (float)Math.sin(Math.toRadians(angle));
            lightPosition[3] = 0.0f;
        }
        else if( 2 == lightAxis )
        {
            lightPosition[1] = 5.0f * (float)Math.sin(Math.toRadians(angle));
            lightPosition[0] = 0.0f;
            lightPosition[2] = 5.0f * (float)Math.cos(Math.toRadians(angle));
            lightPosition[3] = 0.0f;
        }
        else if( 3 == lightAxis )
        {
            lightPosition[0] = 5.0f * (float)Math.sin(Math.toRadians(angle));
            lightPosition[2] = 0.0f;
            lightPosition[1] = 5.0f * (float)Math.cos(Math.toRadians(angle));
            lightPosition[3] = 0.0f;
        }

        
        
        for(int le = 0; le < 24; ++le)
        {
            
            Matrix.setIdentityM(modelMatrix, 0);
            Matrix.setIdentityM(translateMatrix, 0);
            Matrix.setIdentityM(rotateMatrix, 0);
        
            Matrix.translateM(translateMatrix, 0, 1.5f * (le % 6), 1.5f * (le / 6), 0.0f);

            Matrix.rotateM(rotateMatrix, 0, angle, 1.0f, 1.0f, 1.0f);

            Matrix.multiplyMM(modelMatrix, 0,
                     modelMatrix, 0,
                     translateMatrix, 0);

            Matrix.multiplyMM(modelMatrix, 0,
                     modelMatrix, 0,
                     rotateMatrix, 0);
            
            GLES32.glUniformMatrix4fv(mMatrixUniform, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(vMatrixUniform, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(pMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
            
            
            if( toggleLightFlag > 0 )
            {
                GLES32.glUniform1i(uniformLightToggleFlag, toggleLightFlag);
                
                GLES32.glUniform3fv( uniformLightAmbient, 1, lightAmbient, 0 );
                GLES32.glUniform3fv( uniformLightDiffuse, 1, lightDefuse, 0 );
                GLES32.glUniform3fv( uniformLightSpecular, 1, lightSpecular, 0 );
                GLES32.glUniform4fv( uniformPosition, 1, lightPosition, 0 );

                GLES32.glUniform3fv( uniformMaterialAmbient, 1, materialAmbient[le], 0 );
                GLES32.glUniform3fv( uniformMaterialDiffuse, 1, materialDefuse[le], 0 );
                GLES32.glUniform3fv( uniformMaterialSpecular, 1, materialSpecular[le], 0 );
                GLES32.glUniform1f( uniformMaterialShininess, materialShiness[le] );
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
            
        }
        GLES32.glUseProgram(0);
        
        requestRender();
    }
    
    private void update()
    {
        // Code
        
        if(toggleAnimation)
        {
            angle += 0.5f;
        }
        
        if(angle > 360.0f)
        {
            angle -= 360.0f;
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
