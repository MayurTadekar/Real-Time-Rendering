//
//  Pyramid.java
//  
//
//  Created by Mayur Tadekar on 27/11/25.
//

package com.mv.mvwindow;

import     android.opengl.GLES32;

import     java.nio.ByteBuffer;
import     java.nio.ByteOrder;
import     java.nio.FloatBuffer;

public     class     Pyramid
{

    private final float pyramidVertices[] = new float[]
    {
        // FRONT
         0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,

        // RIGHT
        0.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
         
        // BACK
        0.0f,  1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         
        // LEFT
        0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f
    };

    private final float pyramidColor[] = new float[]
    {
        1.0f,     1.0f,     1.0f,
        1.0f,     1.0f,     1.0f,
        1.0f,     1.0f,     1.0f,
        1.0f,     1.0f,     1.0f,
        1.0f,     1.0f,     1.0f,
        1.0f,     1.0f,     1.0f
    };

    private final float pyramidNormals[] = new float[]
    {
        // FRONT
        0.0f,0.447214f,0.894427f,
        0.0f,0.447214f,0.894427f,
        0.0f,0.447214f,0.894427f,

        // RIGHT
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
        0.894427f, 0.447214f, 0.0f,
         
        // BACK
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
        0.0f, 0.447214f, -0.894427f,
         
        // LEFT
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f,
        -0.894427f, 0.447214f, 0.0f
    };

    private int[] vbo = new int[1];
    private int[] vbo_color = new int[1];
    private int[] vbo_normal = new int[1];

    public     Pyramid ()
    {
        GLES32.glGenBuffers(1, vbo, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(    pyramidVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticeBuffer = byteBuffer.asFloatBuffer();

        verticeBuffer.put(pyramidVertices);
        verticeBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            pyramidVertices.length * 4,
                            verticeBuffer,
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(    MyAttributes.MV_ATTRIBUTE_POSITION,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,
                                        0,
                                        0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_POSITION);

         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //    COLOR
        GLES32.glGenBuffers(1, vbo_color, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

        ByteBuffer byteBufferColor = ByteBuffer.allocateDirect( pyramidColor.length * 4 );
        byteBufferColor.order(ByteOrder.nativeOrder());

        FloatBuffer colorBuffer = byteBufferColor.asFloatBuffer();

        colorBuffer.put( pyramidColor );
        colorBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            pyramidColor.length * 4,
                            colorBuffer,
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(    MyAttributes.MV_ATTRIBUTE_COLOR,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,
                                        0,
                                        0);
        GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_COLOR);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        //    Normal
        GLES32.glGenBuffers(1, vbo_normal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal[0]);

        ByteBuffer byteBufferNormal = ByteBuffer.allocateDirect( pyramidNormals.length * 4 );
        byteBufferNormal.order(ByteOrder.nativeOrder());

        FloatBuffer normalBuffer = byteBufferNormal.asFloatBuffer();

        normalBuffer.put( pyramidNormals );
        normalBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            pyramidNormals.length * 4,
                            normalBuffer,
                            GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(    MyAttributes.MV_ATTRIBUTE_NORMAL,
                                        3,
                                        GLES32.GL_FLOAT,
                                        false,
                                        0,
                                        0);
        GLES32.glEnableVertexAttribArray(MyAttributes.MV_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
    }

}
