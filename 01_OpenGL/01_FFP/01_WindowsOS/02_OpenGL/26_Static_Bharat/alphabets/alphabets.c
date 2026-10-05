/**************************************************
 * S.G.M.P
 * @file: 
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2025 MarshallingVoid
 * @date: 24 April 2025 (Thursday) 11:32:47 AM
 **************************************************/
#include "..//common//common.h"

#include "alphabets.h"

#include "alphabet_A.h"
#include "alphabet_B.h"
#include "alphabet_H.h"
#include "alphabet_R.h"
#include "alphabet_T.h"


#define	GOLDENRATIO	1.618033988749

#define	ALPHABET_WIDTH	3.0f
#define	ALPHABET_HEIGHT (ALPHABET_WIDTH*GOLDENRATIO)

#define	ALPHABET_WIDTH_HALF		(ALPHABET_WIDTH/2.0)
#define	ALPHABET_HEIGHT_HALF	(ALPHABET_HEIGHT/2.0)

#define	TOSHOWSTRING	"BHARAT"

#define	MY_RGB(r, g, b)	{r/255.0f, g/255.0f, b/255.0f}

static GLfloat mst_topColor[] = MY_RGB(241.0f, 195.0f, 56.0f);
static GLfloat mst_midColor[] = MY_RGB(255.0f, 255.0f, 255.0f);
static GLfloat mst_bottomColor[] = MY_RGB(0.0f, 255.0f, 0.0f);

static GLfloat	mst_boundBox[]	=	{
										-ALPHABET_WIDTH_HALF,	//	LEFT_X_LIMIT
										ALPHABET_HEIGHT_HALF,	//	TOP_Y_LIMIT
										ALPHABET_WIDTH_HALF,	//	RIGHT_X_LIMIT
										-ALPHABET_HEIGHT_HALF	//	BOTTOM_Y_LIMIT
									};

void 	InitializeAlphabets()
{
	// Code
	mst_InitializeA(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeB(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeH(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeR(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeT(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
}

void 	UninitializeAlphabets()
{
	// Code
	mst_UninitializeA();
	mst_UninitializeB();
	mst_UninitializeH();
	mst_UninitializeR();
	mst_UninitializeT();
}

void 	DrawAlphabets(		GLfloat* boundBox, 
							GLfloat* topColor, 
							GLfloat* midColor, 
							GLfloat* bottomColor)
{
	// Code
	long strLength = strlen(TOSHOWSTRING);

	// float strWidth = (strLength * ( ALPHABET_WIDTH * 1.1f));

	if(strLength & 1)
	{
		int translateFactor = strLength / 2;
		glTranslatef(	-(ALPHABET_WIDTH*1.01f) * translateFactor, 
						0.0f, 
						0.0f);
	}
	else
	{
		int translateFactor = strLength / 2 - 1;
		glTranslatef(	-((ALPHABET_WIDTH*1.01f) * translateFactor + (ALPHABET_WIDTH_HALF*1.01f)), 
						0.0f, 
						0.0f);
	}

	for(int le = 0; le < strLength; ++le)
	{
		glTranslatef( ALPHABET_WIDTH*1.01f * le, 0.0f, 0.0f );
		switch(TOSHOWSTRING[le])
		{
		case 'B':
			mst_DrawAlphabetB(0.12f, mst_boundBox, mst_topColor, mst_midColor, mst_bottomColor);
			break;
		
		case 'H':
			mst_DrawAlphabetH(0.12f, mst_boundBox, mst_topColor, mst_midColor, mst_bottomColor);
			break;

		case 'A':	
			mst_DrawAlphabetA(0.12f, mst_boundBox, mst_topColor, mst_midColor, mst_bottomColor);
			break;		

		case 'R':	
			mst_DrawAlphabetR(0.12f, mst_boundBox, mst_topColor, mst_midColor, mst_bottomColor);
			break;			

		case 'T':	
			mst_DrawAlphabetT(0.12f, mst_boundBox, mst_topColor, mst_midColor, mst_bottomColor);
			break;			
		}
		glTranslatef( -ALPHABET_WIDTH*1.01f * le, 0.0f, 0.0f );
	}



	if(strLength & 1)
	{
		int translateFactor = strLength / 2;
		glTranslatef(	(ALPHABET_WIDTH*1.01f) * translateFactor, 
						0.0f, 
						0.0f);
	}
	else
	{
		int translateFactor = strLength / 2 - 1;
		glTranslatef(	((ALPHABET_WIDTH*1.01f) * translateFactor + (ALPHABET_WIDTH_HALF*1.01f)), 
						0.0f, 
						0.0f);
	}
}

