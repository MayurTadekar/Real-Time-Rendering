/**************************************************
 * S.G.M.P
 * @file: palva_palvi
 * @brief: Implementation of Maams Challenge Video Objects
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2023 MarshallingVoid
 * @date: 14 January 2025 (Tuesday) 07:24:59 PM
 **************************************************/

//	Headers
#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#define	_USE_MATH_DEFINES
#include <math.h>

#define DEG2RAD(x)	(x * M_PI / 180.0f)

//	Macros
#define VALUE_FROM_PERCENT(complete, percent)	(complete * percent / 100.0f)

//	Global Variable Definition
float* scooter_vertices = NULL;


static void Curve(	float x_pos, 
					float y_pos, 
					float percent, 
					int start_point, 
					int end_point, 
					float x_radius, 
					float y_radius, 
					int fill_Curve,
					float delta)
{
	//	Code
	glBegin(GL_TRIANGLE_STRIP);
	{
		for(int le = start_point; le <= end_point; ++le)
		{
			float angle = M_PI * 2.0f * le / 36.0f;

			glColor3f(0.0f, 0.0f, 0.0f);


			if(fill_Curve)
			{
				glVertex3f(	0.0f + x_pos,
							0.0f + y_pos,
							0.0f);
			}
			else
			{
				glVertex3f( VALUE_FROM_PERCENT(x_radius * sinf(angle), percent) + x_pos,
							VALUE_FROM_PERCENT(y_radius * cosf(angle), percent) + y_pos,
							0.0f);
			}

			glVertex3f( VALUE_FROM_PERCENT((x_radius + 0.01f + delta) * sinf(angle), percent) + x_pos,
						VALUE_FROM_PERCENT((y_radius + 0.01f + delta) * cosf(angle), percent) + y_pos,
						0.0f);
		}
	}
	glEnd();
}
	
static void CurveToPoint(	float x_pos, 
							float y_pos, 
							float percent, 
							int start_point, 
							int end_point, 
							float x_radius, 
							float y_radius, 
							float delta,
							float x, float y, float z,
							float r, float g, float b,
							float r_angle)
{
	//	Code
	glBegin(GL_TRIANGLE_STRIP);
	{
		for(int le = start_point; le <= end_point; ++le)
		{
			float angle = M_PI * 2.0f * le / 36.0f;

			glColor3f(r, g, b);

			float _x = VALUE_FROM_PERCENT(x, percent);
			float _y = VALUE_FROM_PERCENT(y, percent);

			glVertex3f(	(_x * cosf(DEG2RAD(r_angle)) + _y * sinf(DEG2RAD(r_angle))) + x_pos,
						(-_x * sinf(DEG2RAD(r_angle)) + _y * cosf(DEG2RAD(r_angle))) + y_pos,
						z);

			_x = VALUE_FROM_PERCENT((x_radius + delta) * sinf(angle), percent);
			_y = VALUE_FROM_PERCENT((y_radius + delta) * cosf(angle), percent);

			glVertex3f( (_x * cosf(DEG2RAD(r_angle)) + _y * sinf(DEG2RAD(r_angle))) + x_pos,
						(-_x * sinf(DEG2RAD(r_angle)) + _y * cosf(DEG2RAD(r_angle))) + y_pos,
						0.0f);
		}
	}
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	{
		for(int le = start_point; le <= end_point; ++le)
		{
			float angle = M_PI * 2.0f * le / 36.0f;

			glColor3f(0.0f, 0.0f, 0.0f);

			float _x = VALUE_FROM_PERCENT((x_radius) * sinf(angle), percent);
			float _y = VALUE_FROM_PERCENT((y_radius) * cosf(angle), percent);

			glVertex3f( (_x * cosf(DEG2RAD(r_angle)) + _y * sinf(DEG2RAD(r_angle))) + x_pos,
						(-_x * sinf(DEG2RAD(r_angle)) + _y * cosf(DEG2RAD(r_angle))) + y_pos,
						0.0f);

			_x = VALUE_FROM_PERCENT((x_radius + delta) * sinf(angle), percent);
			_y = VALUE_FROM_PERCENT((y_radius + delta) * cosf(angle), percent);

			glVertex3f( (_x * cosf(DEG2RAD(r_angle)) + _y * sinf(DEG2RAD(r_angle))) + x_pos,
						(-_x * sinf(DEG2RAD(r_angle)) + _y * cosf(DEG2RAD(r_angle))) + y_pos,
						0.0f);
		}
	}
	glEnd();
}


static void Line(float x_pos, float y_pos, float percent)
{
	// Code
	glBegin(GL_TRIANGLE_STRIP);
	{
		glColor3f(0.0f, 0.0f, 0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.0f, percent) + y_pos,
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(0.01f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.0f, percent) + y_pos, 
					0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.05f, percent) + y_pos, 
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(0.01f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.05f, percent) + y_pos, 
					0.0f);
	}
	glEnd();
}

static void Bike_Driver_Smile(float x_pos, float y_pos, float percent)
{
	// Code
	Curve(x_pos, y_pos, percent, 9, 27, 0.15f, 0.1f, 0, 0.02);
	Curve(x_pos, y_pos, percent, 9, 27, 0.15f, 0.8f, 0, 0.02);
}

extern void Bike_Driver(float x_pos, float y_pos, float percent, int close_eye)
{
	// Code

	Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
			VALUE_FROM_PERCENT(-0.1f, percent) + y_pos, 
			percent, 0, 15, 0.35f, 0.5f, 0, 0.0);
	
	Curve(	VALUE_FROM_PERCENT(-0.05f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.025f, percent) + y_pos, 
			percent, 28, 32, 0.3f, 0.4f, 0, 0.0);

	Curve(	VALUE_FROM_PERCENT(-0.29f, percent) + x_pos,  
			VALUE_FROM_PERCENT(-0.17f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 55), 20, 34, 0.3f, 0.5f, 0, 0.0);

	Line(	VALUE_FROM_PERCENT(-0.25f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.33f, percent) + y_pos, percent);

	Line(	VALUE_FROM_PERCENT(-0.15f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.38f, percent) + y_pos, percent);

	Line(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.4f, percent) + y_pos, percent);

	if(close_eye)
	{
		Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos,  
			VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 5), 0, 36, 0.3f, 0.1f, 1, 0.0);
	}
	else
	{
		Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos,  
			VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 5), 0, 36, 0.3f, 0.4f, 1, 0.0);
	}

	Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos,  
			VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 5), 27, 36, 0.3f, 1.3f, 0, 0.04);

	if(close_eye)
	{
		Curve(	VALUE_FROM_PERCENT(-0.2f, percent) + x_pos,  
				VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
				VALUE_FROM_PERCENT(percent, 5), 0, 36, 0.3f, 0.1f, 1, 0.0);
	}
	else
	{
		Curve(	VALUE_FROM_PERCENT(-0.2f, percent) + x_pos,  
				VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
				VALUE_FROM_PERCENT(percent, 5), 0, 36, 0.3f, 0.4f, 1, 0.0);
	}

	Curve(	VALUE_FROM_PERCENT(-0.2f, percent) + x_pos,  
			VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 5), 0, 9, 0.3f, 1.3f, 0, 0.04);

	Bike_Driver_Smile(	VALUE_FROM_PERCENT(-0.1f, percent) + x_pos,  
						VALUE_FROM_PERCENT(0.0f, percent) + y_pos, 
						VALUE_FROM_PERCENT(percent, 30));

	CurveToPoint(	VALUE_FROM_PERCENT(0.125f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-1.16f, percent) + y_pos, 
					VALUE_FROM_PERCENT(percent, 65), 
					2, 
					18, 
					0.3f, 
					0.7f, 
					0.01f,
					-0.2f, 
					0.17f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	//	Leg
	CurveToPoint(	VALUE_FROM_PERCENT(0.13f, percent) + x_pos,
					VALUE_FROM_PERCENT(-1.97f, percent) + y_pos,
					percent, 
					29, 
					36, 
					0.03f, 
					0.35f, 
					0.01f,
					-0.2f, 
					0.17f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);
	
	CurveToPoint(	VALUE_FROM_PERCENT(-0.05f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-1.97f, percent) + y_pos, 
					percent, 
					29, 
					36, 
					0.03f, 
					0.4f, 
					0.01f,
					0.18f, 
					0.36f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);
	
	CurveToPoint(	VALUE_FROM_PERCENT(0.006f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-1.82f, percent) + y_pos, 
					percent, 
					9, 
					27, 
					0.086f, 
					0.2f, 
					0.01f,
					0.0f, 
					0.1f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	//	Left Hand
	CurveToPoint(	VALUE_FROM_PERCENT(-0.35f, percent) + x_pos,
					VALUE_FROM_PERCENT(-0.68f, percent) + y_pos,
					VALUE_FROM_PERCENT(percent, 80), 
					26, 
					36, 
					.09f, 
					0.30f, 
					0.01f,
					0.0f, 
					.0f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.285f, percent) + x_pos,
					VALUE_FROM_PERCENT(-0.70f, percent) + y_pos,
					VALUE_FROM_PERCENT(percent, 80), 
					26, 
					36, 
					0.06f, 
					0.2f, 
					0.01f,
					-0.17f, 
					0.1f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.381f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-0.718, percent) + y_pos, 
					VALUE_FROM_PERCENT(percent, 80), 
					9, 
					27, 
					0.052f, 
					0.1f, 
					0.01f,
					0.0f, 
					0.1f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);


	//	Right Hand
	CurveToPoint(	VALUE_FROM_PERCENT(-0.01f, percent) + x_pos,
					VALUE_FROM_PERCENT(-0.88f, percent) + y_pos,
					VALUE_FROM_PERCENT(percent, 70), 
					26, 
					36, 
					0.09f, 
					0.30f, 
					0.01f,
					0.0f, 
					0.0f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.06f, percent) + x_pos,
					VALUE_FROM_PERCENT(-0.90f, percent) + y_pos,
					VALUE_FROM_PERCENT(percent, 70), 
					26, 
					36, 
					0.06f, 
					0.2f, 
					0.01f,
					-0.17f, 
					0.1f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.03f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-0.917, percent) + y_pos, 
					VALUE_FROM_PERCENT(percent, 70), 
					9, 
					27, 
					0.058f, 
					0.1f, 
					0.01f,
					0.0f, 
					0.2f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

}

static void Passenger_Hand(float x_pos, float y_pos, float percent)
{
	// Code
	glBegin(GL_TRIANGLE_STRIP);
	{
		glColor3f(0.0f, 0.0f, 0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.0f, percent) + y_pos,
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(0.04f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.0f, percent) + y_pos, 
					0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(-0.20f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.05f, percent) + y_pos, 
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(-0.16f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.05f, percent) + y_pos, 
					0.0f);
	}
	glEnd();
}

extern void Bike_Passenger(float x_pos, float y_pos, float percent, float angel)
{
	// Code

	Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
			VALUE_FROM_PERCENT(-0.1f, percent) + y_pos, 
			percent, 0, 16, 0.35f, 0.5f, 0, 0.0);

	Line(	VALUE_FROM_PERCENT(-0.25f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.33f, percent) + y_pos, percent);

	Line(	VALUE_FROM_PERCENT(-0.15f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.38f, percent) + y_pos, percent);

	Line(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
			VALUE_FROM_PERCENT(0.4f, percent) + y_pos, percent);

	Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos,  
			VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 5), 0, 36, 0.4f, 0.1f, 1, 0.0);

	Curve(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos,  
			VALUE_FROM_PERCENT(0.1f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 5), 27, 36, 0.4f, 0.9f, 0, 0.06);

	Passenger_Hand(	VALUE_FROM_PERCENT(-0.2f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-0.56f, percent) + y_pos, 
					percent);

	Passenger_Hand(	VALUE_FROM_PERCENT(-0.2f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-0.75f, percent) + y_pos, 
					percent);
	
	Curve(	VALUE_FROM_PERCENT(-0.38f, percent) + x_pos,  
			VALUE_FROM_PERCENT(-0.61f, percent) + y_pos, 
			percent, 18, 36, 0.05f, 0.09f, 0, 0.0);

	Curve(	VALUE_FROM_PERCENT(-0.08f, percent) + x_pos,  
			VALUE_FROM_PERCENT(-0.87f, percent) + y_pos, 
			VALUE_FROM_PERCENT(percent, 65), 
			5, 
			16, 
			0.4f, 
			0.7f, 
			0, 
			0.0);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.08f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-0.87f, percent) + y_pos, 
					VALUE_FROM_PERCENT(percent, 65), 
					5, 
					16, 
					0.4f, 
					0.7f, 
					0.01f,
					-0.09f, 
					-0.6f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					0.0f);

	//	Leg
	CurveToPoint(	VALUE_FROM_PERCENT(0.02f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-1.65f, percent) + y_pos, 
					percent, 
					29, 
					36, 
					0.03f, 
					0.35f, 
					0.01f,
					-0.2f, 
					0.17f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					angel);
	
	CurveToPoint(	VALUE_FROM_PERCENT(-0.138f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-1.62f, percent) + y_pos, 
					percent, 
					29, 
					36, 
					0.03f, 
					0.4f, 
					0.01f,
					0.18f, 
					0.35f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					angel);
	
	CurveToPoint(	VALUE_FROM_PERCENT(-0.09f, percent) + x_pos,  
					VALUE_FROM_PERCENT(-1.45f, percent) + y_pos, 
					percent, 
					9, 
					27, 
					0.0770f, 
					0.2f,
					0.01f,
					0.0f, 
					0.1f, 
					0.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f, 
					255.0f / 255.0f,
					angel);
}

static void Scotter_Handle(float x_pos, float y_pos, float percent)
{
	// Code


	glBegin(GL_TRIANGLE_STRIP);
	{
		glColor3f(0.5f, 0.5f, 0.5f);

		glVertex3f(	VALUE_FROM_PERCENT(-0.173f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.06f, percent) + y_pos,
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(-0.173f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.12f, percent) + y_pos,
					0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(-0.138f, percent) + x_pos , 
					VALUE_FROM_PERCENT(0.05, percent) + y_pos,
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(-0.138f, percent) + x_pos , 
					VALUE_FROM_PERCENT(0.11f, percent) + y_pos,
					0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	{
		glColor3f(0.0f, 0.0f, 0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(-0.173f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.06f, percent) + y_pos,
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(-0.173f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.12f, percent) + y_pos,
					0.0f);

		glVertex3f(	VALUE_FROM_PERCENT(-0.138f, percent) + x_pos , 
					VALUE_FROM_PERCENT(0.05, percent) + y_pos,
					0.0f);
		glVertex3f(	VALUE_FROM_PERCENT(-0.138f, percent) + x_pos , 
					VALUE_FROM_PERCENT(0.11f, percent) + y_pos,
					0.0f);
	}
	glEnd();

	CurveToPoint(VALUE_FROM_PERCENT(-0.153f, percent) + x_pos,  
				VALUE_FROM_PERCENT(0.055f, percent) + y_pos, 
				percent, 
				13, 
				29, 
				0.013f, 
				0.001f, 
				0.00f,
				0.0f, 
				0.003f, 
				0.0f,
				0.5f, 
				0.5f, 
				0.5f, 
				10.0f);

	CurveToPoint(VALUE_FROM_PERCENT(-0.153f, percent) + x_pos,  
				VALUE_FROM_PERCENT(0.12f, percent) + y_pos, 
				percent, 
				13, 
				21, 
				0.23f, 
				0.021f, 
				0.002f,
				0.155f, 
				0.019f, 
				0.0f,
				0.5f, 	
				0.5f, 
				0.5f, 
				30.0f);

	CurveToPoint(VALUE_FROM_PERCENT(-0.153f, percent) + x_pos,  
				VALUE_FROM_PERCENT(0.155f, percent) + y_pos, 
				percent, 
				13, 
				21, 
				0.23f, 
				0.021f, 
				0.002f,
				-0.1f, 
				-0.05f, 
				0.0f,
				0.5f, 
				0.5f, 
				0.5f, 
				30.0f);
	
}



static void heart(float x_pos, float y_pos, float percent, float r_angle)
{
	// Code
	glBegin(GL_TRIANGLE_STRIP);
	{

		for (float x = -1.139; x <= 1.139; x += 0.001) 
        {

			glColor3f(1.0f, 0.0f, 0.0f);

			glVertex3f(x_pos, y_pos, 0.0f);

            float delta = cbrt(x*x) * cbrt(x*x) - 4*x*x + 4;
            float y1 = (cbrt(x*x) + sqrt(delta)) / 2;
            float y2 = (cbrt(x*x) - sqrt(delta)) / 2;

            float _x = VALUE_FROM_PERCENT(x, percent);
			float _y = VALUE_FROM_PERCENT(y1, percent);

            glVertex2f(	(_x * cosf(DEG2RAD(r_angle)) + _y * sinf(DEG2RAD(r_angle))) + x_pos,
						(-_x * sinf(DEG2RAD(r_angle)) + _y * cosf(DEG2RAD(r_angle))) + y_pos);

            _x = VALUE_FROM_PERCENT(x, percent);
			_y = VALUE_FROM_PERCENT(y2, percent);

            glVertex2f(	(_x * cosf(DEG2RAD(r_angle)) + _y * sinf(DEG2RAD(r_angle))) + x_pos,
						(-_x * sinf(DEG2RAD(r_angle)) + _y * cosf(DEG2RAD(r_angle))) + y_pos);
        }
	}
	glEnd();
}

static void Scotter_Front(float x_pos, float y_pos, float percent)
{
	// Code
	CurveToPoint(	VALUE_FROM_PERCENT(-0.4f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.1f, percent) + y_pos,
					percent, 
					27, 
					36, 
					0.4f, 
					1.3f, 
					-0.01f, 
					0.0f, 
					1.0f, 
					0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.4f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.09f, percent) + y_pos,
					percent, 
					18, 
					27, 
					0.4f, 
					0.3f, 
					-0.01f, 
					0.0f, 1.0f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);


	CurveToPoint(	VALUE_FROM_PERCENT(-0.29f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.4f, percent) + y_pos,
					percent, 
					27, 
					36, 
					0.1f, 
					1.5f, 
					0.01f, 
					-0.15f, 1.5f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.4f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.1f, percent) + y_pos,
					percent, 
					0, 
					9, 
					0.1f, 
					0.1f, 
					-0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	heart(	VALUE_FROM_PERCENT(-0.56f, percent) + x_pos, 
			VALUE_FROM_PERCENT(-0.55f, percent) + y_pos,
			VALUE_FROM_PERCENT(percent, 10), 10.0f);

}

extern void Scotter_Base(float x_pos, float y_pos, float percent)
{
	// Code

	CurveToPoint(	VALUE_FROM_PERCENT(0.6f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.4f, percent) + y_pos,
					percent, 0, 18, 0.2f, 0.05f, 0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					-50.0f);


	CurveToPoint(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.1f, percent) + y_pos,
					percent, 0, 36, 0.3f, 0.3f, 0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.09f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.4f, percent) + y_pos,
					percent, 18, 27, 0.3f, 0.2f, 0.01f, 
					-0.3f, 0.18f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.09f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.4f, percent) + y_pos,
					percent, 18, 27, 0.3f, 0.2f, 0.01f, 
					0.1f, 0.18f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.29f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.5f, percent) + y_pos,
					percent, 28, 36, 0.1f, 1.6f, 0.01f, 
					-0.01f, 0.0f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.29f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.5f, percent) + y_pos,
					percent, 0, 2, 0.03f, 1.6f, 0.01f, 
					-0.01f, -0.06f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.29f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.5f, percent) + y_pos,
					percent, 2, 8, 0.03f, 1.6f, 0.01f, 
					-0.05f, 0.15f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.05f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.2f, percent) + y_pos,
					percent, 21, 27, 0.3f, 0.1f, 0.01f, 
					-0.4f, -0.237f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.21f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.52f, percent) + y_pos,
					percent, 12, 16, 0.28f, 0.1f, 0.01f, 
					0.22f, 0.22f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					-10.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.11f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.34f, percent) + y_pos,
					percent, 14, 20, 0.5f, 0.1f, 0.01f, 
					-0.055f, 0.15f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					-40.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.245f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.88f, percent) + y_pos,
					percent, 9, 20, 0.75f, 0.3f, 0.01f, 
					-0.4f, 0.02f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					-60.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.41f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.62f, percent) + y_pos,
					percent, 3, 9, 0.44f, 0.3f, 0.01f, 
					-0.5f, 0.0f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					-60.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.37f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.68f, percent) + y_pos,
					percent, 27, 35, 0.2f, 0.41f, 0.01f, 
					-0.22f, -0.4f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					-0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.12f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.68f, percent) + y_pos,
					percent, 9, 18, 0.28f, 0.6f, 0.01f, 
					0.25f, -0.60f, 0.0f, 
					253.0f / 255.0f, 
					253.0f / 255.0f, 
					150.0f / 255.0f,
					0.0f);
}

extern void Scotter(float x_pos, float y_pos, float percent)
{
	// Code

	CurveToPoint(	VALUE_FROM_PERCENT(-0.24f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.57f, percent) + y_pos,
					percent, 0, 36, 0.07f, 0.12f, 0.01f, 
					0.0f, 0.00f, 0.0f, 
					0.0f,  
					0.0f,  
					0.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.2f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.32f, percent) + y_pos,
					percent, 0, 36, 0.07f, 0.12f, 0.01f, 
					0.0f, 0.00f, 0.0f, 
					0.0f,  
					0.0f,  
					0.0f,
					0.0f);

	Scotter_Front(x_pos, y_pos, VALUE_FROM_PERCENT(percent, 40));
	Scotter_Base(x_pos, y_pos, VALUE_FROM_PERCENT(percent, 40));
	Scotter_Handle(	VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.0f, percent) + y_pos, percent);
}


extern void Scene_01(float x_pos, float y_pos, float percent, int close_eye)
{
	// Code


	Scotter(VALUE_FROM_PERCENT(0.1f, percent) + x_pos, 
			VALUE_FROM_PERCENT(-1.0f, percent) + y_pos, percent * 2.0f);

	Bike_Driver(VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
				VALUE_FROM_PERCENT(0.0f, percent) + y_pos, percent, close_eye);
	
	Bike_Passenger(	VALUE_FROM_PERCENT(0.48f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.0f, percent) + y_pos, percent, 0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.72f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.15f, percent) + y_pos,
					percent, 
					0, 
					36, 
					0.02f, 
					0.06f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.9f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.0f, percent) + y_pos,
					percent, 
					0, 
					36,
					0.06f, 
					0.10f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(1.2f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.8f, percent) + y_pos,
					percent, 
					0, 
					36, 
					0.1f, 
					0.14f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(1.2f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.4f, percent) + y_pos,
					percent, 11,
					25, 
					0.4f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.3f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-2.2f, percent) + y_pos,
					percent, 11,
					25, 
					0.2f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.3f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-2.4f, percent) + y_pos,
					percent, 11,
					25, 
					0.05f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.8f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-2.0f, percent) + y_pos,
					percent, 11,
					25, 
					0.05f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);

}


extern void Scene_02(float x_pos, float y_pos, float percent, int close_eye)
{
	// Code


	Scotter(VALUE_FROM_PERCENT(0.1f, percent) + x_pos, 
			VALUE_FROM_PERCENT(-1.0f, percent) + y_pos, percent * 2.0f);

	Bike_Driver(VALUE_FROM_PERCENT(0.0f, percent) + x_pos, 
				VALUE_FROM_PERCENT(0.0f, percent) + y_pos, percent, close_eye);
	
	Bike_Passenger(	VALUE_FROM_PERCENT(0.48f, percent) + x_pos, 
					VALUE_FROM_PERCENT(0.01f, percent) + y_pos, percent, 0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.8f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.1f, percent) + y_pos,
					percent, 
					0, 
					36, 
					0.06f, 
					0.10f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					0.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(1.0f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-0.9f, percent) + y_pos,
					percent, 
					0, 
					36, 
					0.1f, 
					0.14f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					0.0f);


	CurveToPoint(	VALUE_FROM_PERCENT(0.8f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-1.8f, percent) + y_pos,
					percent, 11,
					25, 
					0.3f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(0.1f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-2.4f, percent) + y_pos,
					percent, 11,
					25, 
					0.1f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);

	CurveToPoint(	VALUE_FROM_PERCENT(-0.5f, percent) + x_pos, 
					VALUE_FROM_PERCENT(-2.6f, percent) + y_pos,
					percent, 11,
					25, 
					0.02f, 
					0.01f, 
					0.01f, 
					0.0f,  
					0.0f,  
					0.0f, 
					255.0f / 255.0f,  
					255.0f / 255.0f,  
					255.0f / 255.0f,
					-40.0f);
}

