/**************************************************
 * S.G.M.P
 * @file:
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @copyright ©2020-2025 MarshallingVoid
 **************************************************/

#include ".\common\common.h"
#include ".\alphabets\alphabets.h"
#include ".\fighter_jet.h"
#include ".\ashoka_chakra.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdlib.h>

#define ALPHABET_WIDTH 3.0f
#define ALPHABET_HEIGHT (ALPHABET_WIDTH * 1.618033988749f) // Golden Ratio

#define MAX_JET_LEFT_PART_POSITIONS 200
#define MAX_JET_MIDDLE_PART_POSITIONS 400
#define MAX_JET_RIGHT_PART_POSITIONS 300
#define MAX_JET_PART_POSITIONS (MAX_JET_LEFT_PART_POSITIONS + MAX_JET_MIDDLE_PART_POSITIONS + MAX_JET_RIGHT_PART_POSITIONS)

#define MY_RGB(r, g, b) {r / 255.0f, g / 255.0f, b / 255.0f}

// IAF colors
static GLfloat saffronColor[] = MY_RGB(255.0f, 103.0f, 31.0f); // Saffron for smoke
static GLfloat whiteColor[] = MY_RGB(255.0f, 255.0f, 255.0f); // White for smoke
static GLfloat greenColor[] = MY_RGB(4.0f, 106.0f, 56.0f);	  // Green for smoke

static GLfloat chakra_angle = 0.0f;
static GLfloat chakra_color_factor = 0.0f;
static GLfloat flag_color_factor = 0.0f;

static const GLfloat BHARAT_letter_positions[] = {
	-12.5f, 0.0f,	// B
	-14.0f, 13.0f,	// H
	-14.0f, -13.0f, // A
	14.0f, 13.0f,	// R
	14.0f, -13.0f,	// A
	12.5f, 0.0f		// T
};

static float BHARAT_letter_percentages[] = {
	100.0f, // B
	100.0f, // H
	100.0f, // A
	100.0f, // R
	100.0f, // A
	100.0f	// T
};

static float BHARAT_letter_color_percentages[] = {
	0.0f, // B
	0.0f, // H
	0.0f, // A
	0.0f, // R
	0.0f, // A
	0.0f  // T
};

static GLfloat *topJetPosition = NULL;
static GLfloat *centerJetPosition = NULL;
static GLfloat *bottomJetPosition = NULL;
static long jetPositionIndex = 0;
static GLfloat jetRotationAngle = 90.0f;
static GLfloat jetRotationAngleIncrement = 90.0f / MAX_JET_LEFT_PART_POSITIONS;

static void DrawDynamicBharatJets(void)
{
	// Code

	// Top IAF Jet with saffron smoke
	{
		// Set the position of the jet
		glTranslatef(topJetPosition[jetPositionIndex * 3 + 0], topJetPosition[jetPositionIndex * 3 + 1], topJetPosition[jetPositionIndex * 3 + 2]);
		// Set Scale
		glScalef(0.5f, 0.5f, 0.5f);
		// Set Rotation
		glRotatef(-jetRotationAngle, 0.0f, 0.0f, 1.0f);
		// Draw the IAF jet with saffron smoke
		DrawIAFJet(saffronColor[0], saffronColor[1], saffronColor[2]);
		// Reset rotation
		glRotatef(jetRotationAngle, 0.0f, 0.0f, 1.0f);
		// Reset scale
		glScalef(2.0f, 2.0f, 2.0f);
		// Reset position
		glTranslatef(-topJetPosition[jetPositionIndex * 3 + 0], -topJetPosition[jetPositionIndex * 3 + 1], -topJetPosition[jetPositionIndex * 3 + 2]);
	}

	// Center IAF Jet with white smoke
	{
		// Set the position of the jet
		glTranslatef(centerJetPosition[jetPositionIndex * 3 + 0], centerJetPosition[jetPositionIndex * 3 + 1], centerJetPosition[jetPositionIndex * 3 + 2]);
		// Set Scale
		glScalef(0.5f, 0.5f, 0.5f);
		DrawIAFJet(whiteColor[0], whiteColor[1], whiteColor[2]);
		// Reset scale
		glScalef(2.0f, 2.0f, 2.0f);
		// Reset position
		glTranslatef(-centerJetPosition[jetPositionIndex * 3 + 0], -centerJetPosition[jetPositionIndex * 3 + 1], -centerJetPosition[jetPositionIndex * 3 + 2]);
	}

	// Right IAF Jet with green smoke
	{
		// Set the position of the jet
		glTranslatef(bottomJetPosition[jetPositionIndex * 3 + 0], bottomJetPosition[jetPositionIndex * 3 + 1], bottomJetPosition[jetPositionIndex * 3 + 2]);
		// Set Scale
		glScalef(0.5f, 0.5f, 0.5f);
		// Set Rotation
		glRotatef(jetRotationAngle, 0.0f, 0.0f, 1.0f);
		// Draw the IAF jet with saffron smoke
		DrawIAFJet(greenColor[0], greenColor[1], greenColor[2]);
		// Reset rotation
		glRotatef(-jetRotationAngle, 0.0f, 0.0f, 1.0f);
		// Reset scale
		glScalef(2.0f, 2.0f, 2.0f);
		// Reset position
		glTranslatef(-bottomJetPosition[jetPositionIndex * 3 + 0], -bottomJetPosition[jetPositionIndex * 3 + 1], -bottomJetPosition[jetPositionIndex * 3 + 2]);
	}
}

extern void InitializeDynamicBharat(void)
{
	// Code

	// Initialize the jet positions
	topJetPosition = (GLfloat *)malloc(MAX_JET_PART_POSITIONS * 3 * sizeof(GLfloat));
	centerJetPosition = (GLfloat *)malloc(MAX_JET_PART_POSITIONS * 3 * sizeof(GLfloat));
	bottomJetPosition = (GLfloat *)malloc(MAX_JET_PART_POSITIONS * 3 * sizeof(GLfloat));

	const GLfloat jetXOffset = 12.0f;
	const GLfloat jetYOffset = 11.5f;

	// Left Part Positions for the jets to make them look like a formation
	for (int le = 0; le < MAX_JET_LEFT_PART_POSITIONS; le++)
	{
		float angle = (float)(le * 2.0f * M_PI / (MAX_JET_LEFT_PART_POSITIONS * 4));

		topJetPosition[le * 3 + 0] = 5.0f * -cos(angle) - jetXOffset;
		topJetPosition[le * 3 + 1] = 9.0f * -sin(angle) + jetYOffset;
		topJetPosition[le * 3 + 2] = 0.0f;

		centerJetPosition[le * 3 + 0] = -8.0f + (le * (8.0f / MAX_JET_LEFT_PART_POSITIONS)) - jetXOffset;
		centerJetPosition[le * 3 + 1] = 0.0f;
		centerJetPosition[le * 3 + 2] = 0.0f;

		bottomJetPosition[le * 3 + 0] = 5.0f * -cos(angle) - jetXOffset;
		bottomJetPosition[le * 3 + 1] = 9.0f * sin(angle) - jetYOffset;
		bottomJetPosition[le * 3 + 2] = 0.0f;
	}

	// Center Part Positions for the jets to make go from left to right
	for (int le = MAX_JET_LEFT_PART_POSITIONS; le < (MAX_JET_LEFT_PART_POSITIONS + MAX_JET_MIDDLE_PART_POSITIONS); le++)
	{
		int leIndex = le - MAX_JET_LEFT_PART_POSITIONS;

		topJetPosition[le * 3 + 0] = (leIndex * (24.0f / MAX_JET_MIDDLE_PART_POSITIONS)) - jetXOffset;
		topJetPosition[le * 3 + 1] = 2.5f;
		topJetPosition[le * 3 + 2] = 0.0f;

		centerJetPosition[le * 3 + 0] = (leIndex * (24.0f / MAX_JET_MIDDLE_PART_POSITIONS)) - jetXOffset;
		centerJetPosition[le * 3 + 1] = 0.0f;
		centerJetPosition[le * 3 + 2] = 0.0f;

		bottomJetPosition[le * 3 + 0] = (leIndex * (24.0f / MAX_JET_MIDDLE_PART_POSITIONS)) - jetXOffset;
		bottomJetPosition[le * 3 + 1] = -2.5f;
		bottomJetPosition[le * 3 + 2] = 0.0f;
	}

	// Right Part Positions for the jets to make them look like a formation
	for (int le = (MAX_JET_LEFT_PART_POSITIONS + MAX_JET_MIDDLE_PART_POSITIONS); le < MAX_JET_PART_POSITIONS; le++)
	{
		int leIndex = le - (MAX_JET_LEFT_PART_POSITIONS + MAX_JET_MIDDLE_PART_POSITIONS);

		topJetPosition[le * 3 + 0] = 7.0f * sin(leIndex * 2.0f * M_PI / (MAX_JET_RIGHT_PART_POSITIONS * 3)) + jetXOffset;
		topJetPosition[le * 3 + 1] = 9.0f * -cos(leIndex * 2.0f * M_PI / (MAX_JET_RIGHT_PART_POSITIONS * 3)) + jetYOffset;
		topJetPosition[le * 3 + 2] = 0.0f;

		centerJetPosition[le * 3 + 0] = (leIndex * (10.0f / MAX_JET_RIGHT_PART_POSITIONS)) + jetXOffset;
		centerJetPosition[le * 3 + 1] = 0.0f;
		centerJetPosition[le * 3 + 2] = 0.0f;

		bottomJetPosition[le * 3 + 0] = 7.0f * sin(leIndex * 2.0f * M_PI / (MAX_JET_RIGHT_PART_POSITIONS * 3)) + jetXOffset;
		bottomJetPosition[le * 3 + 1] = 9.0f * cos(leIndex * 2.0f * M_PI / (MAX_JET_RIGHT_PART_POSITIONS * 3)) - jetYOffset;
		bottomJetPosition[le * 3 + 2] = 0.0f;
	}

	InitSmokeParticles();

	InitializeAlphabets();
}

extern void DrawDynamicBharat(void)
{
	// Code
	DrawAlphabets(BHARAT_letter_positions, BHARAT_letter_percentages, BHARAT_letter_color_percentages);

	DrawDynamicBharatJets();
	
	DrawAshokaChakra(chakra_angle, chakra_color_factor, flag_color_factor);
}

static BOOL UpdateColorPercentage(int mask)
{
	// Code
	BOOL isColorPercentageComplete = FALSE;

	if (mask & 0x01)
	{
		if (BHARAT_letter_color_percentages[0] < 1.0f)
			BHARAT_letter_color_percentages[0] += 0.02f;
		else
			BHARAT_letter_color_percentages[0] = 1.0f;
	}

	if (mask & 0x02)
	{
		if (BHARAT_letter_color_percentages[1] < 1.0f)
			BHARAT_letter_color_percentages[1] += 0.02f;
		else
			BHARAT_letter_color_percentages[1] = 1.0f;
	}

	if (mask & 0x04)
	{
		if (BHARAT_letter_color_percentages[2] < 1.0f)
			BHARAT_letter_color_percentages[2] += 0.02f;
		else
			BHARAT_letter_color_percentages[2] = 1.0f;
	}

	if (mask & 0x08)
	{
		if (BHARAT_letter_color_percentages[3] < 1.0f)
			BHARAT_letter_color_percentages[3] += 0.02f;
		else
			BHARAT_letter_color_percentages[3] = 1.0f;
	}

	if (mask & 0x10)
	{
		if (BHARAT_letter_color_percentages[4] < 1.0f)
			BHARAT_letter_color_percentages[4] += 0.02f;
		else
			BHARAT_letter_color_percentages[4] = 1.0f;
	}

	if (mask & 0x20)
	{
		if (BHARAT_letter_color_percentages[5] < 1.0f)
			BHARAT_letter_color_percentages[5] += 0.02f;
		else
		{
			BHARAT_letter_color_percentages[5] = 1.0f;
			isColorPercentageComplete = TRUE;
		}
	}
	return (isColorPercentageComplete);
}

extern void UpdateDynamicBharat(void)
{
	// Code
	static int animationCount = 0;

	switch (animationCount)
	{
	case 0:
		animationCount++;
		break;

	case 1:
		if (BHARAT_letter_percentages[0] > 0.0f)
		{
			BHARAT_letter_percentages[0] -= 0.5f;
		}
		else
		{
			BHARAT_letter_percentages[0] = 0.0f;
			animationCount++;
		}
		break;

	case 2:
		if (BHARAT_letter_percentages[1] > 0.0f)
		{
			BHARAT_letter_percentages[1] -= 0.5f;
		}
		else
		{
			BHARAT_letter_percentages[1] = 0.0f;
			animationCount++;
		}
		break;

	case 3:
		if (BHARAT_letter_percentages[2] > 0.0f)
		{
			BHARAT_letter_percentages[2] -= 0.5f;
		}
		else
		{
			BHARAT_letter_percentages[2] = 0.0f;
			animationCount++;
		}
		break;

	case 4:
		if (BHARAT_letter_percentages[3] > 0.0f)
		{
			BHARAT_letter_percentages[3] -= 0.5f;
		}
		else
		{
			BHARAT_letter_percentages[3] = 0.0f;
			animationCount++;
		}
		break;

	case 5:
		if (BHARAT_letter_percentages[4] > 0.0f)
		{
			BHARAT_letter_percentages[4] -= 0.5f;
		}
		else
		{
			BHARAT_letter_percentages[4] = 0.0f;
			animationCount++;
		}
		break;

	case 6:
		if (BHARAT_letter_percentages[5] > 0.0f)
		{
			BHARAT_letter_percentages[5] -= 0.5f;
		}
		else
		{
			BHARAT_letter_percentages[5] = 0.0f;
			animationCount++;
		}
		break;

	case 7:
		if (jetPositionIndex < MAX_JET_LEFT_PART_POSITIONS - 1)
		{
			jetRotationAngle -= jetRotationAngleIncrement;
			jetPositionIndex++;
		}
		else
		{
			jetPositionIndex = MAX_JET_LEFT_PART_POSITIONS - 1;
			animationCount++;
		}
		break;

	case 8:
		if (centerJetPosition[jetPositionIndex * 3 + 0] < -(ALPHABET_WIDTH * 2.0f))
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 9:
		UpdateColorPercentage(0x01);
		if (centerJetPosition[jetPositionIndex * 3 + 0] < -(ALPHABET_WIDTH * 1.0f))
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 10:
		UpdateColorPercentage(0x01 | 0x02);
		if (centerJetPosition[jetPositionIndex * 3 + 0] < 0.0f)
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 11:
		UpdateColorPercentage(0x01 | 0x02 | 0x04);
		if (centerJetPosition[jetPositionIndex * 3 + 0] < (ALPHABET_WIDTH * 1.0f))
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 12:
		UpdateColorPercentage(0x01 | 0x02 | 0x04 | 0x08);
		if (centerJetPosition[jetPositionIndex * 3 + 0] < (ALPHABET_WIDTH * 2.0f))
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 13:
		UpdateColorPercentage(0x01 | 0x02 | 0x04 | 0x08 | 0x10);
		if (centerJetPosition[jetPositionIndex * 3 + 0] < (ALPHABET_WIDTH * 3.0f))
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 14:
		UpdateColorPercentage(0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20);
		if (centerJetPosition[jetPositionIndex * 3 + 0] < (ALPHABET_WIDTH * 1.0f))
		{
			jetPositionIndex++;
		}
		else
		{
			animationCount++;
		}
		break;

	case 15:
		UpdateColorPercentage(0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20);

		if (jetPositionIndex < (MAX_JET_LEFT_PART_POSITIONS + MAX_JET_MIDDLE_PART_POSITIONS - 1))
		{
			jetPositionIndex++;
		}
		else
		{
			jetPositionIndex = (MAX_JET_LEFT_PART_POSITIONS + MAX_JET_MIDDLE_PART_POSITIONS - 1);
			animationCount++;
		}
		break;

	case 16:
		UpdateColorPercentage(0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20);

		if (jetPositionIndex < MAX_JET_PART_POSITIONS - 1)
		{
			jetRotationAngle -= jetRotationAngleIncrement;
			jetPositionIndex++;
		}
		else
		{
			jetPositionIndex = MAX_JET_PART_POSITIONS - 1;
			animationCount++;
		}
		break;

	case 17:
		
		if(chakra_color_factor < 1.0f)
			chakra_color_factor += 0.0025f;
		else
			animationCount++;

		break;

	case 18:
		if(flag_color_factor < 1.0f)
			flag_color_factor += 0.0025f;
		else
			animationCount++;

	default:
		break;
	}

	if(chakra_color_factor > 0.0f)
	{
		chakra_angle += 0.5f;
		if(chakra_angle >= 360.0f)
			chakra_angle -= 360.0f;
	}
}

extern void UninitializeDynamicBharat(void)
{
	// Code
	UninitializeAlphabets();
}
