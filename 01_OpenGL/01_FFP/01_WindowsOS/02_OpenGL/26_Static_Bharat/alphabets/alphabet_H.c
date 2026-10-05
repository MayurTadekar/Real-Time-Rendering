/**************************************************
 * S.G.M.P
 * @file: 
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 24 April 2025 (Thursday) 11:55:00 AM
 **************************************************/
 

#include "..//common//common.h"

#define	_USE_MATH_DEFINES
#include "math.h" 

#include "..//pathtracer.h"

#define	NR_SECTIONS	1
static GLint	mst_pathSectionsfor[] = {2 + 19 + 10 + 10 + 19 + 2 + 1};
static GLfloat*	mst_pathfor = NULL;

extern	void	mst_InitializeH(GLfloat width, GLfloat height)
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
	*(mst_pathfor + index * 3 + 1)	= VALUE_FROM_PERCENT(height, 100);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= -VALUE_FROM_PERCENT(width, 92);
	*(mst_pathfor + index * 3 + 1)	= VALUE_FROM_PERCENT(height, 0);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	GLfloat x, y;

	for(int le = 0; le <= 18; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * -sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x - VALUE_FROM_PERCENT(width, 82);
		*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 88.8);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	// *(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 10);
	// *(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 89);
	// *(mst_pathfor + index * 3 + 2)	= 0.0f;
	// index++;

	for(int le = 0; le <= 9; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x - VALUE_FROM_PERCENT(width, 62);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 4);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	for(int le = 9; le <= 18; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * -sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 62);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 16.5);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}


	for(int le = 0; le <= 18; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 82);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 88.8);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 82);
	*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 0);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 82);
	*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 100);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 82);
	*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 101);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

}

extern	void	mst_UninitializeH()
{
	// Code
	if(mst_pathfor)
	{
		free(mst_pathfor);
		mst_pathfor = NULL;
	}
}

extern	void 	mst_DrawAlphabetH(	GLfloat width,
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