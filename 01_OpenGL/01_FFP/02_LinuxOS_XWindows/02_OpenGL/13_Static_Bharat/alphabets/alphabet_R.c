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
static GLint	mst_pathSectionsfor[] = {2 + 10 + 19 + 15 + 1 + 1};
static GLfloat*	mst_pathfor = NULL;

extern	void	mst_InitializeR(GLfloat width, GLfloat height)
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
	*(mst_pathfor + index * 3 + 1)	= VALUE_FROM_PERCENT(height, 0);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	GLfloat x, y;

	for(int le = 0; le <= 9; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * -cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x - VALUE_FROM_PERCENT(width, 82);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 88);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}

	// *(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 10);
	// *(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 89);
	// *(mst_pathfor + index * 3 + 2)	= 0.0f;
	// index++;

	for(int le = 9; le <= 27; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 60) * -cosf(angle);
		y = VALUE_FROM_PERCENT(width, 71.8) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 0);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 50);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}


	for(int le = 9; le <= 23; ++le)
	{
		GLfloat angle = M_PI * 2.0f * le / 36;

		x = VALUE_FROM_PERCENT(width, 10) * cosf(angle);
		y = VALUE_FROM_PERCENT(width, 10) * sinf(angle);

		*(mst_pathfor + index * 3 + 0)	= x - VALUE_FROM_PERCENT(width, 55);
		*(mst_pathfor + index * 3 + 1)	= y + VALUE_FROM_PERCENT(height, 0);
		*(mst_pathfor + index * 3 + 2)	= 0.0f;
		index++;
	}


	// for(int le = 9; le <= 27; ++le)
	// {
	// 	GLfloat angle = M_PI * 2.0f * le / 36;

	// 	x = VALUE_FROM_PERCENT(width, 60) * -cosf(angle);
	// 	y = VALUE_FROM_PERCENT(width, 71.8) * sinf(angle);

	// 	*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 20);
	// 	*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 50.5);
	// 	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	// 	index++;
	// }

	*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 99);
	*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 95);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

	*(mst_pathfor + index * 3 + 0)	= x + VALUE_FROM_PERCENT(width, 99);
	*(mst_pathfor + index * 3 + 1)	= y - VALUE_FROM_PERCENT(height, 100);
	*(mst_pathfor + index * 3 + 2)	= 0.0f;
	index++;

}

extern	void	mst_UninitializeR()
{
	// Code
	if(mst_pathfor)
	{
		free(mst_pathfor);
		mst_pathfor = NULL;
	}
}

extern	void 	mst_DrawAlphabetR(	GLfloat width,
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