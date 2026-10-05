/////////////////////////////////////////
// Created By : Aishwarya Kendle
// HOW TO USE:
//
// Declare Variable
// Sphere* sphere;
//
// Add Below Lines in initialize (Creating Object)
// sphere = [[Sphere alloc]init];
// [sphere initialize:0.5 :18 :18];
//
// // create vbo for position & Similaraly for texcoord & Normals
//	glGenBuffers(1, &vbo_Sphere_Position);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
//	glBufferData(GL_ARRAY_BUFFER, [sphere getNumberOfSphereVertices] * sizeof(GLfloat), [sphere getSphereVertex], GL_STATIC_DRAW);
//	glVertexAttribPointer(PRJ_ATRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(PRJ_ATRIBUTE_POSITION);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
// // Display - DrawArrays Call Should be
//
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, [sphere getNumberOfSphereVertices]);
//
///////////////////////////////////////
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <math.h>

@interface Sphere : NSObject
@end

@implementation Sphere
{
@private
    int VertexPointer;
    int TexcoordPointer;
    int NormalPointer;
    int MAX_VERTICES;
    int MAX_TEXCOORDS;
    int MAX_NORMALS;
    int CACHE_SIZE;
    int MAX_ARRAY_SIZE;

    float *model_Vertices;
    float *model_Texcoord;
    float *model_Normal;
}

- (void)initialize:(double)radius :(int)slices :(int)stacks
{
    VertexPointer = 0;
    TexcoordPointer = 0;
    NormalPointer = 0;

    MAX_VERTICES = 100000;
    MAX_TEXCOORDS = 100000;
    MAX_NORMALS = 100000;
    CACHE_SIZE = 240;

    model_Vertices = (float *)malloc(MAX_VERTICES * sizeof(float));
    model_Texcoord = (float *)malloc(MAX_TEXCOORDS * sizeof(float));
    model_Normal = (float *)malloc(MAX_NORMALS * sizeof(float));

    [self generateSphereData1:radius:slices:stacks];
}

- (void)uninitialize
{
    free(model_Vertices);
    model_Vertices = NULL;

    free(model_Texcoord);
    model_Texcoord = NULL;

    free(model_Normal);
    model_Normal = NULL;
}

- (void)addTexcoord:(float)a :(float)b
{
    model_Texcoord[TexcoordPointer++] = a;
    model_Texcoord[TexcoordPointer++] = b;
}

- (void)addVertices:(float)x :(float)y :(float)z
{
    model_Vertices[VertexPointer++] = x;
    model_Vertices[VertexPointer++] = y;
    model_Vertices[VertexPointer++] = z;
}

- (void)addNormals:(float)p :(float)q :(float)r
{
    model_Normal[NormalPointer++] = p;
    model_Normal[NormalPointer++] = q;
    model_Normal[NormalPointer++] = r;
}

- (void)generateSphereData1:(double)radius :(int)slices :(int)stacks
{
    int k = 0;
    int kk = 0;
    int i, j;
    float sinCache1a[CACHE_SIZE];
    float cosCache1a[CACHE_SIZE];
    float sinCache2a[CACHE_SIZE];
    float cosCache2a[CACHE_SIZE];
    float sinCache3a[CACHE_SIZE];
    float cosCache3a[CACHE_SIZE];
    float sinCache1b[CACHE_SIZE];
    float cosCache1b[CACHE_SIZE];
    float sinCache2b[CACHE_SIZE];
    float cosCache2b[CACHE_SIZE];
    float sinCache3b[CACHE_SIZE];
    float cosCache3b[CACHE_SIZE];
    float angle;
    float zLow, zHigh;
    float sintemp1 = 0.0, sintemp2 = 0.0, sintemp3 = 0.0, sintemp4 = 0.0;
    float costemp1 = 0.0, costemp2 = 0.0, costemp3 = 0.0, costemp4 = 0.0;
    int start, finish;
    if (slices >= CACHE_SIZE)
        slices = CACHE_SIZE - 1;
    if (stacks >= CACHE_SIZE)
        stacks = CACHE_SIZE - 1;
    if (slices < 2 || stacks < 1 || radius < 0.0)
    {
        return;
    }
    for (i = 0; i < slices; i++)
    {
        angle = 2 * (float)M_PI * i / slices;
        sinCache1a[i] = (float)sin(angle);
        cosCache1a[i] = (float)cos(angle);
        sinCache2a[i] = sinCache1a[i];
        cosCache2a[i] = cosCache1a[i];
    }
    for (j = 0; j <= stacks; j++)
    {
        angle = (float)M_PI * j / stacks;
        sinCache2b[j] = (float)sin(angle);
        cosCache2b[j] = (float)cos(angle);
        sinCache1b[j] = radius * (float)sin(angle);
        cosCache1b[j] = radius * (float)cos(angle);
    }
    /* Make sure it comes to a point */
    sinCache1b[0] = 0;
    sinCache1b[stacks] = 0;
    sinCache1a[slices] = sinCache1a[0];
    cosCache1a[slices] = cosCache1a[0];
    sinCache2a[slices] = sinCache2a[0];
    cosCache2a[slices] = cosCache2a[0];
    start = 0;
    finish = stacks;
    for (j = start; j < finish; j++)
    {
        zLow = cosCache1b[j];
        zHigh = cosCache1b[j + 1];
        sintemp1 = sinCache1b[j];
        sintemp2 = sinCache1b[j + 1];
        sintemp3 = sinCache2b[j + 1];
        costemp3 = cosCache2b[j + 1];
        sintemp4 = sinCache2b[j];
        costemp4 = cosCache2b[j];
        // fprintf(gpFile, "Slices are %d\n", slices);
        for (i = 0; i <= slices; i++)
        {
            [self addVertices:sintemp2 * sinCache1a[i]:sintemp2 * cosCache1a[i]:zHigh];
            [self addVertices:sintemp1 * sinCache1a[i]:sintemp1 * cosCache1a[i]:zLow];
            [self addNormals:sinCache2a[i] * sintemp3:cosCache2a[i] * sintemp3:costemp3];
            [self addNormals:sinCache2a[i] * sintemp4:cosCache2a[i] * sintemp4:costemp4];
            [self addTexcoord:1 - (float)i / slices:1 - (float)(j + 1) / stacks];
            [self addTexcoord:1 - (float)i / slices:1 - (float)(j) / stacks];
        }
    }
}

- (int)getNumberOfSphereVertices
{
    return (VertexPointer);
}

- (int)getNumberOfSphereTexcoord
{
    return (TexcoordPointer);
}

- (int)getNumberOfSphereNormal
{
    return (NormalPointer);
}

- (float *)getSphereVertex
{
    return (model_Vertices);
}

- (float *)getSphereTexcoord
{
    return (model_Texcoord);
}

- (float *)getSphereNormal
{
    return (model_Normal);
}
@end
