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

#define GOLDENRATIO 1.618033988749

#define ALPHABET_WIDTH 3.0f
#define ALPHABET_HEIGHT (ALPHABET_WIDTH * GOLDENRATIO)

#define ALPHABET_WIDTH_HALF (ALPHABET_WIDTH / 2.0)
#define ALPHABET_HEIGHT_HALF (ALPHABET_HEIGHT / 2.0)

#define TOSHOWSTRING "BHARAT"

#define MY_RGB(r, g, b) {r / 255.0f, g / 255.0f, b / 255.0f}

static GLfloat mst_topColor[] = MY_RGB(241.0f, 195.0f, 56.0f);
static GLfloat mst_midColor[] = MY_RGB(255.0f, 255.0f, 255.0f);
static GLfloat mst_bottomColor[] = MY_RGB(0.0f, 255.0f, 0.0f);

static GLfloat mst_boundBox[] = {
	-ALPHABET_WIDTH_HALF, //	LEFT_X_LIMIT
	ALPHABET_HEIGHT_HALF, //	TOP_Y_LIMIT
	ALPHABET_WIDTH_HALF,  //	RIGHT_X_LIMIT
	-ALPHABET_HEIGHT_HALF //	BOTTOM_Y_LIMIT
};

static GLfloat lerp(GLfloat start, GLfloat end, GLfloat t)
{
	return (start * t + end * (1.0f - t));
}

void InitializeAlphabets()
{
	// Code
	mst_InitializeA(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeB(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeH(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeR(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
	mst_InitializeT(ALPHABET_WIDTH_HALF, ALPHABET_HEIGHT_HALF);
}

void UninitializeAlphabets()
{
	// Code
	mst_UninitializeA();
	mst_UninitializeB();
	mst_UninitializeH();
	mst_UninitializeR();
	mst_UninitializeT();
}

void DrawAlphabets(const GLfloat *positions,
				   GLfloat *percentage,
				   GLfloat *alphabetColorPercentage)
{
	// Local Variable Declarations
	GLfloat B_topColor[] = {
		lerp(mst_topColor[0], 1.0f, alphabetColorPercentage[0]),
		lerp(mst_topColor[1], 1.0f, alphabetColorPercentage[0]),
		lerp(mst_topColor[2], 1.0f, alphabetColorPercentage[0])};
	GLfloat B_midColor[] = {
		lerp(mst_midColor[0], 1.0f, alphabetColorPercentage[0]),
		lerp(mst_midColor[1], 1.0f, alphabetColorPercentage[0]),
		lerp(mst_midColor[2], 1.0f, alphabetColorPercentage[0])};
	GLfloat B_bottomColor[] = {
		lerp(mst_bottomColor[0], 1.0f, alphabetColorPercentage[0]),
		lerp(mst_bottomColor[1], 1.0f, alphabetColorPercentage[0]),
		lerp(mst_bottomColor[2], 1.0f, alphabetColorPercentage[0])};

	GLfloat H_topColor[] = {
		lerp(mst_topColor[0], 1.0f, alphabetColorPercentage[1]),
		lerp(mst_topColor[1], 1.0f, alphabetColorPercentage[1]),
		lerp(mst_topColor[2], 1.0f, alphabetColorPercentage[1])};
	GLfloat H_midColor[] = {
		lerp(mst_midColor[0], 1.0f, alphabetColorPercentage[1]),
		lerp(mst_midColor[1], 1.0f, alphabetColorPercentage[1]),
		lerp(mst_midColor[2], 1.0f, alphabetColorPercentage[1])};
	GLfloat H_bottomColor[] = {
		lerp(mst_bottomColor[0], 1.0f, alphabetColorPercentage[1]),
		lerp(mst_bottomColor[1], 1.0f, alphabetColorPercentage[1]),
		lerp(mst_bottomColor[2], 1.0f, alphabetColorPercentage[1])};
	GLfloat A_topColor[] = {
		lerp(mst_topColor[0], 1.0f, alphabetColorPercentage[2]),
		lerp(mst_topColor[1], 1.0f, alphabetColorPercentage[2]),
		lerp(mst_topColor[2], 1.0f, alphabetColorPercentage[2])};
	GLfloat A_midColor[] = {
		lerp(mst_midColor[0], 1.0f, alphabetColorPercentage[2]),
		lerp(mst_midColor[1], 1.0f, alphabetColorPercentage[2]),
		lerp(mst_midColor[2], 1.0f, alphabetColorPercentage[2])};
	GLfloat A_bottomColor[] = {
		lerp(mst_bottomColor[0], 1.0f, alphabetColorPercentage[2]),
		lerp(mst_bottomColor[1], 1.0f, alphabetColorPercentage[2]),
		lerp(mst_bottomColor[2], 1.0f, alphabetColorPercentage[2])};
	GLfloat R_topColor[] = {
		lerp(mst_topColor[0], 1.0f, alphabetColorPercentage[3]),
		lerp(mst_topColor[1], 1.0f, alphabetColorPercentage[3]),
		lerp(mst_topColor[2], 1.0f, alphabetColorPercentage[3])};
	GLfloat R_midColor[] = {
		lerp(mst_midColor[0], 1.0f, alphabetColorPercentage[3]),
		lerp(mst_midColor[1], 1.0f, alphabetColorPercentage[3]),
		lerp(mst_midColor[2], 1.0f, alphabetColorPercentage[3])};
	GLfloat R_bottomColor[] = {
		lerp(mst_bottomColor[0], 1.0f, alphabetColorPercentage[3]),
		lerp(mst_bottomColor[1], 1.0f, alphabetColorPercentage[3]),
		lerp(mst_bottomColor[2], 1.0f, alphabetColorPercentage[3])};
	GLfloat A_topColor_2[] = {
		lerp(mst_topColor[0], 1.0f, alphabetColorPercentage[4]),
		lerp(mst_topColor[1], 1.0f, alphabetColorPercentage[4]),
		lerp(mst_topColor[2], 1.0f, alphabetColorPercentage[4])};
	GLfloat A_midColor_2[] = {
		lerp(mst_midColor[0], 1.0f, alphabetColorPercentage[4]),
		lerp(mst_midColor[1], 1.0f, alphabetColorPercentage[4]),
		lerp(mst_midColor[2], 1.0f, alphabetColorPercentage[4])};
	GLfloat A_bottomColor_2[] = {
		lerp(mst_bottomColor[0], 1.0f, alphabetColorPercentage[4]),
		lerp(mst_bottomColor[1], 1.0f, alphabetColorPercentage[4]),
		lerp(mst_bottomColor[2], 1.0f, alphabetColorPercentage[4])};
	GLfloat T_topColor[] = {
		lerp(mst_topColor[0], 1.0f, alphabetColorPercentage[5]),
		lerp(mst_topColor[1], 1.0f, alphabetColorPercentage[5]),
		lerp(mst_topColor[2], 1.0f, alphabetColorPercentage[5])};
	GLfloat T_midColor[] = {
		lerp(mst_midColor[0], 1.0f, alphabetColorPercentage[5]),
		lerp(mst_midColor[1], 1.0f, alphabetColorPercentage[5]),
		lerp(mst_midColor[2], 1.0f, alphabetColorPercentage[5])};
	GLfloat T_bottomColor[] = {
		lerp(mst_bottomColor[0], 1.0f, alphabetColorPercentage[5]),
		lerp(mst_bottomColor[1], 1.0f, alphabetColorPercentage[5]),
		lerp(mst_bottomColor[2], 1.0f, alphabetColorPercentage[5])};

	// Code
	long strLength = strlen(TOSHOWSTRING);

	if (strLength & 1)
	{
		int translateFactor = strLength / 2;
		glTranslatef(-(ALPHABET_WIDTH * 1.01f) * translateFactor,
					 0.0f,
					 0.0f);
	}
	else
	{
		int translateFactor = strLength / 2 - 1;
		glTranslatef(-((ALPHABET_WIDTH * 1.01f) * translateFactor + (ALPHABET_WIDTH_HALF * 1.01f)),
					 0.0f,
					 0.0f);
	}

	int le = 0;
	glTranslatef(ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	glTranslatef(
		VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	mst_DrawAlphabetB(0.12f, mst_boundBox, B_topColor, B_midColor, B_bottomColor);
	glTranslatef(
		-VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		-VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	glTranslatef(-ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	le++;

	glTranslatef(ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	glTranslatef(
		VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	mst_DrawAlphabetH(0.12f, mst_boundBox, H_topColor, H_midColor, H_bottomColor);
	glTranslatef(
		-VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		-VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	glTranslatef(-ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	le++;

	glTranslatef(ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	glTranslatef(
		VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	mst_DrawAlphabetA(0.12f, mst_boundBox, A_topColor, A_midColor, A_bottomColor);
	glTranslatef(
		-VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		-VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	glTranslatef(-ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	le++;

	glTranslatef(ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	glTranslatef(
		VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	mst_DrawAlphabetR(0.12f, mst_boundBox, R_topColor, R_midColor, R_bottomColor);
	glTranslatef(
		-VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		-VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	glTranslatef(-ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	le++;

	glTranslatef(ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	glTranslatef(
		VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	mst_DrawAlphabetA(0.12f, mst_boundBox, A_topColor_2, A_midColor_2, A_bottomColor_2);
	glTranslatef(
		-VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		-VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	glTranslatef(-ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	le++;

	glTranslatef(ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);
	glTranslatef(
		VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	mst_DrawAlphabetT(0.12f, mst_boundBox, T_topColor, T_midColor, T_bottomColor);
	glTranslatef(
		-VALUE_FROM_PERCENT(positions[le * 2 + 0], percentage[le]),
		-VALUE_FROM_PERCENT(positions[le * 2 + 1], percentage[le]),
		0.0f);
	glTranslatef(-ALPHABET_WIDTH * 1.01f * le, 0.0f, 0.0f);

	if (strLength & 1)
	{
		int translateFactor = strLength / 2;
		glTranslatef((ALPHABET_WIDTH * 1.01f) * translateFactor,
					 0.0f,
					 0.0f);
	}
	else
	{
		int translateFactor = strLength / 2 - 1;
		glTranslatef(((ALPHABET_WIDTH * 1.01f) * translateFactor + (ALPHABET_WIDTH_HALF * 1.01f)),
					 0.0f,
					 0.0f);
	}
}
