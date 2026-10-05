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

public class GLESView    extends GLSurfaceView
                        implements     GLSurfaceView.Renderer,
                                    OnDoubleTapListener,
                                    OnGestureListener{

    private final Context context;

    private GestureDetector gestureDetector;

    private ShaderLoader vertexShader;
    private ShaderLoader fragmentShader;
    private ShaderProgramObject  shaderProgramObject;

    private int[] vao = new int[2];
    private int[] vbo = new int[1];
    private int[] vbo_color = new int[1];

    private float     la[] = new float[] { 0.1f, 0.1f, 0.1f};
    private int     la_uniform[] = new int[2];

    private float     ld_red[] = new float[] { 1.0f, 0.0f, 0.0f};
    private float     ld_blue[] = new float[] { 0.0f, 0.0f, 1.0f};
    private int     ld_uniform[] = new int[2];

    private float     ls_red[] = new float[] { 1.0f, 0.0f, 0.0f};
    private float     ls_blue[] = new float[] { 0.0f, 0.0f, 1.0f};
    private int     ls_uniform[] = new int[2];

    private float     lp_red[] = new float[] { 10.0f, 0.0f, 0.0f, 1.0f};
    private float     lp_blue[] = new float[] { -10.0f, 0.0f, 0.0f, 1.0f};
    private int     lp_uniform[] = new int[2];

    private float     ka[] = new float[] { 1.0f, 1.0f, 1.0f};
    private int     ka_uniform;

    private float     kd[] = new float[] { 1.0f, 1.0f, 1.0f};
    private int     kd_uniform;

    private float     ks[] = new float[] { 1.0f, 1.0f, 1.0f};
    private int     ks_uniform;

    private float     kshine = 128.0f;
    private int     kshine_uniform;

    private int     uniformLightToggle;

    private int mUniform;
    private int vUniform;
    private int pUniform;

    private float perspectiveProjectionMatrix[] = new float[16];    //    4x4 Matrix

    boolean toggleLight = false;
    boolean toggleAnimation = false;
    
    public GLESView(Context context) {
        super(context);

        this.context = context;

        setEGLContextClientVersion(3);

        setRenderer(this);

        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        gestureDetector = new GestureDetector(context, this, null, false);

        gestureDetector.setOnDoubleTapListener(this);
    }
    /**
        Overridden methods of GLSurfaceView.Renderer
    */
    /***********************/

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String version = gl.glGetString(GL10.GL_VERSION);
        System.out.println("MST: Version" + version);
        
        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("VDG: GLSL Version = " + glslVersion);

        initialize(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        draw();
        
        update();
    }

    /***********************/

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        //    CODE
        int     eventAction = event.getAction();
        if(  !gestureDetector.onTouchEvent(event) )
            super.onTouchEvent(event);

        return(true);
    }


    @Override
    public boolean onDoubleTap(MotionEvent event) {
        //    CODE
        toggleAnimation = !toggleAnimation;
        return(true);
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        //    CODE
        return(true);
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
        //    CODE

        return(true);
    }

    @Override
    public boolean onDown(MotionEvent event) {
        //    CODE
        return(true);
    }

    @Override
    public void onLongPress(MotionEvent event) {
        //    CODE
        toggleLight = !toggleLight;
    }

    @Override
    public boolean onFling(MotionEvent event_1, MotionEvent event_2, float velocityX, float velocityY) {
        return(true);
    }

    @Override
    public boolean onScroll(MotionEvent event_1, MotionEvent event_2, float distanceX, float distanceY) {
        System.out.println("MST: Exiting...");
        System.exit(0);
        return(true);
    }

    @Override
    public void onShowPress(MotionEvent event) {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent event) {
        return(true);
    }


    /******************** MY METHODS ************************/
    private void initialize(GL10 gl) {

        //    ************************************************
        //    Vertex Shader
        //    ************************************************
        vertexShader = new ShaderLoader(    GLES32.GL_VERTEX_SHADER,
                                            "Vertex Shader",
                                            VertexShader.getShaderSource()
                                        );
        
        //    ************************************************


        //    ************************************************
        //    Fragment Shader
        //    ************************************************
        fragmentShader = new ShaderLoader(    GLES32.GL_FRAGMENT_SHADER,
                                            "Fragment Shader",
                                            FragmentShader.getShaderSource()
                                        );
        
        //    ************************************************


        //    ************************************************
        //    Shader Program Object
        //    ************************************************
        shaderProgramObject = new ShaderProgramObject(    vertexShader.getShaderObject(),
                                                        fragmentShader.getShaderObject()
                                                    );
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_POSITION, "vPosition");
        //GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_COLOR, "vColor");
        GLES32.glBindAttribLocation(shaderProgramObject.getObject(), MyAttributes.MV_ATTRIBUTE_NORMAL, "vNormal");

        shaderProgramObject.linkObject();

        
        mUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_mMatrix");
        vUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_vMatrix");
        pUniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_pMatrix");

        uniformLightToggle = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "toggleLight");

        la_uniform[0] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_la[0]");
        la_uniform[1] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_la[1]");

        ld_uniform[0] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ld[0]");
        ld_uniform[1] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ld[1]");

        ls_uniform[0] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ls[0]");
        ls_uniform[1] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ls[1]");

        lp_uniform[0] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_lp[0]");
        lp_uniform[1] = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_lp[1]");

        ka_uniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ka");
        ks_uniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_ks");
        kd_uniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_kd");

        kshine_uniform = GLES32.glGetUniformLocation(shaderProgramObject.getObject(), "u_kshiny");

        

        //    *************************************************************
        //    Verteices, Color, Shader Attribs, VBO, VAO Initializations here
        //    *************************************************************

        GLES32.glGenVertexArrays(1, vao, 0);

        Pyramid pyramid;

        GLES32.glBindVertexArray(vao[0]);
        {
            pyramid = new Pyramid();
        }
        GLES32.glBindVertexArray(0);


        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        GLES32.glClearDepthf(1.0f);

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void resize(int width, int height) {
        GLES32.glViewport(0, 0, width, height);

        float     ratio = (float) width / (float) height;

        /*Matrix.frustumM(     perspectiveProjectionMatrix,
                            0,
                            -ratio,
                            ratio,
                            -1f,
                            1f,
                            0.1f,
                            7f
                );*/
        Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, ratio, 0.1f, 100.0f);

    }

    private float tri_angle = 0.0f;

    public void draw() {

        //    VARIABLE
        float     modelMatrix[] = new float[16];
        float     viewMatrix[] = new float[16];
        float     rotationMatrix[] = new float[16];
        float     translateMatrix[] = new float[16];
        float     modelViewProjectionMatrix[] = new float[16];
        float     modelViewProjectionMatrix_2[] = new float[16];

        //    CODE
        GLES32.glClear(    GLES32.GL_COLOR_BUFFER_BIT     |
                        GLES32.GL_DEPTH_BUFFER_BIT );

        GLES32.glUseProgram(shaderProgramObject.getObject());
        {
            Matrix.setIdentityM(modelMatrix, 0);
            Matrix.setIdentityM(viewMatrix, 0);
            Matrix.setIdentityM(rotationMatrix, 0);
            Matrix.setIdentityM(translateMatrix, 0);
            Matrix.setIdentityM(modelViewProjectionMatrix, 0);
            Matrix.setIdentityM(modelViewProjectionMatrix_2, 0);

            Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -5.0f);
            Matrix.rotateM(rotationMatrix, 0, tri_angle, 0f, 1f, 0f );

            Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translateMatrix, 0);
            Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotationMatrix, 0);

            GLES32.glUniformMatrix4fv( mUniform, 1, false, modelMatrix, 0 );
            GLES32.glUniformMatrix4fv( vUniform, 1, false, viewMatrix, 0 );
            GLES32.glUniformMatrix4fv( pUniform, 1, false, perspectiveProjectionMatrix, 0 );

            if( toggleLight )
            {
                GLES32.glUniform1i(uniformLightToggle, 1);

                GLES32.glUniform3fv(la_uniform[0], 1, la, 0);
                GLES32.glUniform3fv(la_uniform[1], 1, la, 0);

                GLES32.glUniform3fv(ld_uniform[0], 1, ld_red, 0);
                GLES32.glUniform3fv(ld_uniform[1], 1, ld_blue, 0);

                GLES32.glUniform3fv(ls_uniform[0], 1, ls_red, 0);
                GLES32.glUniform3fv(ls_uniform[1], 1, ls_blue, 0);
                
                GLES32.glUniform4fv(lp_uniform[0], 1, lp_red, 0);
                GLES32.glUniform4fv(lp_uniform[1], 1, lp_blue, 0);
                
                GLES32.glUniform3fv(ks_uniform, 1, ks, 0);
                GLES32.glUniform3fv(kd_uniform, 1, kd, 0);
                GLES32.glUniform3fv(ka_uniform, 1, ka, 0);
                GLES32.glUniform1f(kshine_uniform, kshine);
            }
            else
            {
                GLES32.glUniform1i(uniformLightToggle, 0);
            }

            GLES32.glBindVertexArray(vao[0]);
            {
                GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);
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
            tri_angle += 1.0f;
            if( tri_angle >= 360.0f )
            {
                tri_angle = 0.0f;
            }
        }
    }

    private void Uninitialize()
    {
        //    CODE
        if( vao[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }

        if( vbo[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vbo, 0);
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
    /********************************************************/
}


