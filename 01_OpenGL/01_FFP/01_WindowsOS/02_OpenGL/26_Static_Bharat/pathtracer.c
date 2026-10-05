/**************************************************
 * S.G.M.P
 * @file: 
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 24 April 2025 (Thursday) 11:41:47 AM
 **************************************************/

#include ".//common//common.h"

#include <math.h>
#include "lerpbox.h"

#define SQR(x) ((x)*(x))

static void mst_GenerateVerticesForPoint(	GLfloat* toDrawVer1, 
											GLfloat* toDrawVer2, 
											GLfloat* ver1, 
											GLfloat* ver2, 
											GLfloat width,
											GLfloat* boundBox)
{
	// Code
	GLfloat front[] = {	
						*(ver2+0) - *(ver1+0),
						*(ver2+1) - *(ver1+1),
						*(ver2+2) - *(ver1+2)
					};

	GLfloat mag = sqrt( SQR(front[0]) + SQR(front[1]) );

	front[0] = front[0] / mag;
	front[1] = front[1] / mag;

	GLfloat right[] = {	-front[1], front[0], front[2] };

	GLfloat right_x_coord = right[0] * width + *(ver1+0);
	GLfloat right_y_coord = right[1] * width + *(ver1+1);

	GLfloat left_x_coord = -right[0] * width + *(ver1+0);
	GLfloat left_y_coord = -right[1] * width + *(ver1+1);

	*(toDrawVer1+0) = right_x_coord;
	*(toDrawVer1+1) = right_y_coord;
	*(toDrawVer1+2) = right[2] * width + *(ver1+2);

	*(toDrawVer2+0) = left_x_coord;
	*(toDrawVer2+1) = left_y_coord;
	*(toDrawVer2+2) = -right[2] * width + *(ver1+2);
}

static void mst_GetColorBasedOnYCoord(	GLfloat *color, 
										GLfloat ycoord, 
										GLfloat* boundBox, 
										GLfloat* bottomColor, 
										GLfloat* topColor)
{
	// Code
	GLfloat bottomColorPercent = (ycoord * 50.0f) / boundBox[3];
	
	bottomColorPercent *= 2;

	GLfloat topColorPercent = 100.0f - bottomColorPercent;



	color[0] = 	VALUE_FROM_PERCENT(bottomColor[0], bottomColorPercent) + 
				VALUE_FROM_PERCENT(topColor[0], topColorPercent);

	color[1] = 	VALUE_FROM_PERCENT(bottomColor[1], bottomColorPercent) + 
				VALUE_FROM_PERCENT(topColor[1], topColorPercent);

	color[2] = 	VALUE_FROM_PERCENT(bottomColor[2], bottomColorPercent) + 
				VALUE_FROM_PERCENT(topColor[2], topColorPercent);
}

extern void	mst_PathTracer(	GLfloat* path, 
							size_t nrPoints, 
							size_t startIndex,
							GLfloat width, 
							GLfloat* boundBox, 
							GLfloat* topColor, 
							GLfloat* midColor, 
							GLfloat* bottomColor)
{
	// Code
	if(nrPoints < 1)
		return;

	GLfloat ver1[3];
	GLfloat ver2[3];

	// BOUND BOX
	// glBegin(GL_LINE_LOOP);
	// {
	// 	glColor3f(1.0f, 0.0f, 0.0f);
	// 	glVertex2f(boundBox[0], boundBox[1]);
	// 	glVertex2f(boundBox[2], boundBox[1]);
	// 	glVertex2f(boundBox[2], boundBox[3]);
	// 	glVertex2f(boundBox[0], boundBox[3]);
	// }
	// glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	// glBegin(GL_LINE_STRIP);
	{
		// glColor3f(1.0f, 1.0f, 1.0f);
		for(long long le = 0; le < nrPoints-1; ++le)
		{

			glColor3f(1.0f, 1.0f, 1.0f);

			int index = startIndex+le;
			mst_GenerateVerticesForPoint( 	ver1,
											ver2,
										 	path + index * 3,
										 	path + (index+1) * 3,
											width,
											boundBox
										);

			setVertexColor(	ver1[1], boundBox[1], 0.0f, boundBox[3],
							topColor[0], topColor[1], topColor[2],
							midColor[0], midColor[1], midColor[2], 
							bottomColor[0], bottomColor[1], bottomColor[2]);

			glVertex3f(	ver1[0],
						ver1[1],
						ver1[2]);


			setVertexColor(	ver2[1], boundBox[1], 0.0f, boundBox[3],
							topColor[0], topColor[1], topColor[2],
							midColor[0], midColor[1], midColor[2], 
							bottomColor[0], bottomColor[1], bottomColor[2]);
			glVertex3f(	ver2[0],
						ver2[1],
						ver2[2]);
		}
	}
	glEnd();
}

