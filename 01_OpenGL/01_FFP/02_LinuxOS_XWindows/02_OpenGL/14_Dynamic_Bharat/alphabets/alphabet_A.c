/**************************************************
 * S.G.M.P
 * @file: 
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 24 April 2025 (Thursday) 11:38:03 AM
 **************************************************/
 

#include "..//common//common.h"

#define	_USE_MATH_DEFINES
#include "math.h" 

#include "..//pathtracer.h"

#define	NR_SECTIONS	1
static GLint	mst_pathSectionsfor[] = {3 + 12 + 1 + 20 + 12 + 1 + 1};
static GLfloat*	mst_pathfor = NULL;

extern	void	mst_InitializeA(GLfloat width, GLfloat height)
{
	// Code
	size_t nrVertices = 0;
	for(int le = 0; le < NR_SECTIONS; ++le)
	{
		nrVertices += mst_pathSectionsfor[le];
	}

	mst_pathfor = (GLfloat*) malloc(sizeof(GLfloat) * nrVertices * 3);

	int index = 0;

	*(mst_pathfor + index * 3 + 0)	= -VALUE_FROM_PERCENT(width, 92);
	*(mst_pathfor + index * 3 + 1)	= -VALUE_FROM_PERCENT(height, 100);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= -VALUE_FROM_PERCENT(width, 92);
	*(mst_pathfor + index * 3 + 1)	= -VALUE_FROM_PERCENT(height, 97.8);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= -VALUE_FROM_PERCENT(width, 50);
	*(mst_pathfor + index * 3 + 1)	= VALUE_FROM_PERCENT(height, 0);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	for(int le = 3; le < 15; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		GLfloat x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		GLfloat y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);


		*(mst_pathfor + index * 3 + 0)	= x;//VALUE_FROM_PERCENT(width, 100);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 88);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	*(mst_pathfor + index * 3 + 0)	= VALUE_FROM_PERCENT(width, 50);
	*(mst_pathfor + index * 3 + 1)	= VALUE_FROM_PERCENT(height, 0);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	for(int le = 15; le <= 15 + 20; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		GLfloat x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		GLfloat y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 80);
		*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 88.8);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	for(int le = 0; le < 11; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 20);
		*(mst_pathfor + index * 3 + 1)	= y;// - VALUE_FROM_PERCENT(height, 81);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	*(mst_pathfor + index * 3 + 0)	= x - VALUE_FROM_PERCENT(width, 30);
	*(mst_pathfor + index * 3 + 1)	= y;//VALUE_FROM_PERCENT(height, 6.5);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= -VALUE_FROM_PERCENT(width, 50);
	*(mst_pathfor + index * 3 + 1)	= VALUE_FROM_PERCENT(height, 5);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;
}

extern	void	mst_UninitializeA()
{
	// Code
	if(mst_pathfor)
	{
		free(mst_pathfor);
		mst_pathfor = NULL;
	}
}

extern	void 	mst_DrawAlphabetA(	GLfloat width,
									GLfloat* boundBox, 
									GLfloat* topColor, 
									GLfloat* midColor, 
									GLfloat* bottomColor)
{
	// Code
	size_t startIndex = 0;
	for(int le = 0; le < NR_SECTIONS; ++le)
	{
		mst_PathTracer(	mst_pathfor, 
						mst_pathSectionsfor[le], 
						startIndex,
						width, 
						boundBox,
						topColor,
						midColor,
						bottomColor);

		startIndex += mst_pathSectionsfor[le];
	}
}
