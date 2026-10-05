/**************************************************
 * S.G.M.P
 * @file:
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 27 April 2025 (Sunday) 04:59:53 PM
 **************************************************/

// Header
#include ".\\common\\common.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "ashoka_chakra.h"

void DrawFlag(GLfloat percent)
{
	// Code

	// Drawing the flag

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	{	
		glTranslatef(0.0f, 0.0f, 1.0f);
		glScalef(15.0f, 15.0f, 15.0f);
		glBegin(GL_TRIANGLE_STRIP);
		{
			glColor4f(
				255.0f / 255.0f,
				103.0f / 255.0f,
				31.0f / 255.0f,
				percent
				);
			glVertex3f(1.5f, 0.25f, 0.0f);
			glVertex3f(-1.5f, 0.25f, 0.0f);
			glVertex3f(1.5f, 0.75f, 0.0f);
			glVertex3f(-1.5f, 0.75f, 0.0f);
		}
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		{
			glColor4f(
				255.0f / 255.0f,
				255.0f / 255.0f,
				255.0f / 255.0f,
				percent
				);
			glVertex3f(-1.5f, 0.25f, 0.0f);
			glVertex3f(1.5f, 0.25f, 0.0f);
			glVertex3f(-1.5f, -0.25f, 0.0f);
			glVertex3f(1.5f, -0.25f, 0.0f);
		}
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		{
			glColor4f(
				4.0f / 255.0f,
				106.0f / 255.0f,
				56.0f / 255.0f,
				percent
				);
			glVertex3f(1.5f, -0.25f, 0.0f);
			glVertex3f(-1.5f, -0.25f, 0.0f);
			glVertex3f(1.5f, -0.75f, 0.0f);
			glVertex3f(-1.5f, -0.75f, 0.0f);
		}
		glEnd();
	}
	glPopMatrix();

	glDisable(GL_BLEND);
}

void DrawAshokaChakra(GLfloat rotate_angle, GLfloat chakra_percent, GLfloat flag_percent)
{
	//  Code
	DrawFlag(flag_percent);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	{
		glRotatef(-rotate_angle, 0.0f, 0.0f, 1.0f);

		glLineWidth(4.0f);
		for (int le = 0; le < 36; ++le)
		{
			GLfloat angle = le * 2.0f * M_PI / 36;

			glBegin(GL_LINES);
			{
				
				glColor4f(
					6.0f / 255.0f,
					0.0f / 255.0f,
					141.0f / 255.0f,
					chakra_percent
					);

				glVertex3f(0.0f, 0.0f, 0.0f);

				glVertex3f(3.5f * sinf(angle),
						   3.5f * cosf(angle),
						   0.0f);
			}
			glEnd();
		}

		glBegin(GL_LINE_LOOP);
		{
			for (int le = 0; le <= 36; ++le)
			{
				GLfloat angle = le * 2.0f * M_PI / 36;
				
				glColor4f(
					6.0f / 255.0f,
					0.0f / 255.0f,
					141.0f / 255.0f,
					chakra_percent
					);

				glVertex3f(3.5f * sinf(angle),
						   3.5f * cosf(angle),
						   0.0f);
			}
		}
		glEnd();

		glLineWidth(1.0f);
	}
	glPopMatrix();

	glDisable(GL_BLEND);
}
