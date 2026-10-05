/**************************************************
 * S.G.M.P
 * @file: 
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 24 April 2025 (Thursday) 
 **************************************************/


#include ".//common//common.h"

#include <math.h>
#include "lerpbox.h"

static GLfloat lerp(GLfloat v0, GLfloat v1, GLfloat t)
{
	// Code
	return( (1.0f-t) * v0 + t * v1 );
}

static GLfloat GetNormalizedT(GLfloat in_fYPos, GLfloat in_fUpperBound, GLfloat in_fLowerBound)
{
	// Code
	if( in_fUpperBound < 0.0f && in_fLowerBound < 0.0f)
	{
		float upperBound = (-1.0f * in_fLowerBound) - (-1 * in_fUpperBound);
		float ypos = (-1.0f * in_fLowerBound) - (-1.0f * in_fYPos);

		return(ypos / upperBound);
	}
	else if(in_fUpperBound <= 0.0f && in_fLowerBound <= 0.0f)
	{

		float upperBound = (in_fUpperBound + (-1.0f * in_fLowerBound));

		float ypos = 0.0f;

		if(in_fYPos >= 0.0f)
		{
			ypos = in_fYPos + (-1.0f * in_fLowerBound);
		}
		else
		{
			ypos = (-1.0f * in_fLowerBound) - (-1.0f * in_fYPos);
		}

		return(ypos / upperBound);
	}
	else
	{
		float upperBound = in_fUpperBound - in_fLowerBound;
		float ypos = in_fYPos - in_fLowerBound;

		return( ypos / upperBound);
	}
}

extern void setVertexColor(	GLfloat in_fYPos, 
							GLfloat in_fUpperBound, GLfloat in_fMiddlePoint, GLfloat in_fLowerBound,
							GLfloat in_fUpperRed, GLfloat in_fUpperGreen, GLfloat in_fUpperBlue,
							GLfloat in_fMidRed, GLfloat in_fMidGreen, GLfloat in_fMidBlue,
							GLfloat in_fLowerRed, GLfloat in_fLowerGreen, GLfloat in_fLowerBlue)
{
	// Code

	GLfloat normalized_t = 0.0f;

	if( in_fUpperBound >= in_fYPos && in_fYPos > in_fMiddlePoint)
	{
		normalized_t = GetNormalizedT(in_fYPos, in_fUpperBound, in_fMiddlePoint);
		
		GLfloat interpolatedRed = lerp(in_fMidRed, in_fUpperRed, normalized_t);
		GLfloat interpolatedGreen = lerp(in_fMidGreen, in_fUpperGreen, normalized_t);
		GLfloat interpolatedBlue = lerp(in_fMidBlue, in_fUpperBlue, normalized_t);

		glColor3f(interpolatedRed, interpolatedGreen, interpolatedBlue);
	}
	else if( in_fMiddlePoint >= in_fYPos && in_fYPos >= in_fLowerBound)
	{
		normalized_t = GetNormalizedT(in_fYPos, in_fMiddlePoint, in_fLowerBound);
		
		GLfloat interpolatedRed = lerp(in_fLowerRed, in_fMidRed, normalized_t);
		GLfloat interpolatedGreen = lerp(in_fLowerGreen, in_fMidGreen, normalized_t);
		GLfloat interpolatedBlue = lerp(in_fLowerBlue, in_fMidBlue, normalized_t);
		
		glColor3f(interpolatedRed, interpolatedGreen, interpolatedBlue);
	}
}

