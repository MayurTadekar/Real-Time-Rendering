/**************************************************
 * S.G.M.P
 * @file: fighter_jet.c
 * @brief: Draws a realistic 2D top-view of an IAF fighter jet
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @copyright ©2020-2025 MarshallingVoid
 * @date:
 **************************************************/

#include "./common/common.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "fighter_jet.h"

// Maximum number of smoke particles
#define MAX_SMOKE_PARTICLES 500

// Smoke particle structure
typedef struct
{
	float x, y;	   // Position
	float dx, dy;  // Velocity
	float size;	   // Size
	float alpha;   // Opacity
	float life;	   // Current life
	float maxLife; // Maximum life
	int active;	   // Active flag
} SmokeParticle;

// Global variables for smoke
static SmokeParticle smokeParticles[MAX_SMOKE_PARTICLES];
static float lastTime = 0.0f;
static int initialized = 0;

// Helper function to draw a circle
static void DrawCircle(float x, float y, float radius, const GLfloat color[3])
{
	glColor3fv(color);
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex2f(x, y);				  // center
		for (int i = 0; i <= 360; i += 5) // Use smaller step for smoother circle
		{
			float radian = i * (M_PI / 180.0f);
			glVertex2f(x + radius * cos(radian), y + radius * sin(radian));
		}
	}
	glEnd();
}

// Helper function to draw IAF roundel
static void DrawIAFRoundel(float x, float y, float radius)
{
	const GLfloat orangeColor[3] = {1.0f, 0.5f, 0.0f}; // Orange for outer ring
	const GLfloat whiteColor[3] = {1.0f, 1.0f, 1.0f};  // White for middle ring
	const GLfloat greenColor[3] = {0.0f, 0.6f, 0.0f};  // Green for center (corrected from blue)

	// Draw outer circle (orange)
	DrawCircle(x, y, radius, orangeColor);

	// Draw middle circle (white)
	DrawCircle(x, y, radius * 0.7f, whiteColor);

	// Draw inner circle (green)
	DrawCircle(x, y, radius * 0.4f, greenColor);
}

// Create a new smoke particle at exhaust position
static void CreateSmokeParticle(float x, float y)
{
	for (int i = 0; i < MAX_SMOKE_PARTICLES; i++)
	{
		if (!smokeParticles[i].active)
		{
			// Random variation for more natural smoke
			float randX = ((float)rand() / RAND_MAX * 0.04f) - 0.02f;
			float randY = ((float)rand() / RAND_MAX * 0.04f) - 0.02f;
			float randSize = ((float)rand() / RAND_MAX * 0.03f) + 0.03f;

			smokeParticles[i].x = x + randX;
			smokeParticles[i].y = y;
			smokeParticles[i].dx = randX * 0.5f;
			smokeParticles[i].dy = -0.2f - ((float)rand() / RAND_MAX * 0.1f); // Trailing behind exhaust
			smokeParticles[i].size = randSize;
			smokeParticles[i].alpha = 0.8f;
			smokeParticles[i].life = 0.0f;
			smokeParticles[i].maxLife = 1.0f + ((float)rand() / RAND_MAX * 0.5f);
			smokeParticles[i].active = 1;

			break;
		}
	}
}

// Update smoke particles
static void UpdateSmokeParticles()
{
	float currentTime = (float)clock() / CLOCKS_PER_SEC;
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	// Create new smoke particles at exhaust points
	if ((int)(currentTime * 20) % 3 == 0)
	{
		// Left exhaust
		CreateSmokeParticle(-0.115f, -3.0f);
		// Right exhaust
		CreateSmokeParticle(0.115f, -3.0f);
	}

	// Update existing particles
	for (int i = 0; i < MAX_SMOKE_PARTICLES; i++)
	{
		if (smokeParticles[i].active)
		{
			// Update position
			smokeParticles[i].x += smokeParticles[i].dx * deltaTime * 6.0f;
			smokeParticles[i].y += smokeParticles[i].dy * deltaTime * 6.0f;

			// Update life and properties
			smokeParticles[i].life += deltaTime;

			if (smokeParticles[i].life >= smokeParticles[i].maxLife)
			{
				smokeParticles[i].active = 0;
			}
			else
			{
				// Particle grows initially then fades
				float lifeRatio = smokeParticles[i].life / smokeParticles[i].maxLife;
				smokeParticles[i].size += deltaTime * 0.6f;			 // Grow over time
				smokeParticles[i].alpha = 0.8f * (1.0f - lifeRatio); // Fade out
			}
		}
	}
}

// Render smoke particles
static void RenderSmokeParticles(GLfloat smoke_red, GLfloat smoke_green, GLfloat smoke_blue)
{
	// Enable blending for smoke effect
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	for (int i = 0; i < MAX_SMOKE_PARTICLES; i++)
	{
		if (smokeParticles[i].active)
		{
			// Set the color for the smoke particles
			glColor4f(smoke_red, smoke_green, smoke_blue, smokeParticles[i].alpha);

			float halfSize = smokeParticles[i].size / 2.0f;
			glVertex2f(smokeParticles[i].x - halfSize, smokeParticles[i].y - halfSize);
			glVertex2f(smokeParticles[i].x + halfSize, smokeParticles[i].y - halfSize);
			glVertex2f(smokeParticles[i].x + halfSize, smokeParticles[i].y + halfSize);
			glVertex2f(smokeParticles[i].x - halfSize, smokeParticles[i].y + halfSize);
		}
	}
	glEnd();

	glDisable(GL_BLEND);
}

// Initialize smoke particle system
void InitSmokeParticles()
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX_SMOKE_PARTICLES; i++)
	{
		smokeParticles[i].active = 0;
	}
	initialized = 1;
	lastTime = (float)clock() / CLOCKS_PER_SEC;
}

void DrawIAFJet(GLfloat smoke_red, GLfloat smoke_green, GLfloat smoke_blue)
{

	// Rotate the jet right side 90
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);

	// Update smoke animation
	UpdateSmokeParticles();

	// Draw smoke behind jet
	RenderSmokeParticles(smoke_red, smoke_green, smoke_blue); // Pass the custom color to the function

	// IAF colors
	const GLfloat iafGrey[3] = {0.35f, 0.35f, 0.35f};		  // Main body color
	const GLfloat iafLightGrey[3] = {0.6f, 0.6f, 0.6f};		  // Lighter areas
	const GLfloat iafDarkGrey[3] = {0.2f, 0.2f, 0.2f};		  // Darker areas
	const GLfloat cockpitColor[3] = {0.1f, 0.5f, 0.9f};		  // Canopy blue
	const GLfloat canopyFrame[3] = {0.1f, 0.1f, 0.1f};		  // Dark frame for canopy
	const GLfloat exhaustColor[3] = {0.8f, 0.3f, 0.0f};		  // Engine exhaust
	const GLfloat exhaustInner[3] = {1.0f, 0.8f, 0.0f};		  // Inner exhaust flame
	const GLfloat whiteColor[3] = {1.0f, 1.0f, 1.0f};		  // White marking
	const GLfloat blackColor[3] = {0.0f, 0.0f, 0.0f};		  // Black details
	const GLfloat navLightRed[3] = {1.0f, 0.0f, 0.0f};		  // Navigation light (red)
	const GLfloat navLightGreen[3] = {0.0f, 1.0f, 0.0f};	  // Navigation light (green)
	const GLfloat missileColor[3] = {0.4f, 0.4f, 0.4f};		  // Weapons
	const GLfloat missileDetailColor[3] = {0.3f, 0.3f, 0.3f}; // Weapon details
	const GLfloat metalSilver[3] = {0.8f, 0.8f, 0.8f};		  // Metallic parts

	// MAIN FUSELAGE (CENTRAL BODY)
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafGrey);
		glVertex2f(0.0f, 2.2f);
		glVertex2f(-0.15f, 1.9f);
		glVertex2f(-0.2f, 1.5f);
		glVertex2f(-0.25f, 1.0f);
		glVertex2f(-0.3f, 0.0f);
		glVertex2f(-0.28f, -0.5f);
		glVertex2f(-0.27f, -1.0f);
		glVertex2f(-0.25f, -1.5f);
		glVertex2f(-0.22f, -2.0f);
		glVertex2f(-0.2f, -2.5f);
		glVertex2f(0.0f, -2.7f);
		glVertex2f(0.2f, -2.5f);
		glVertex2f(0.22f, -2.0f);
		glVertex2f(0.25f, -1.5f);
		glVertex2f(0.27f, -1.0f);
		glVertex2f(0.28f, -0.5f);
		glVertex2f(0.3f, 0.0f);
		glVertex2f(0.25f, 1.0f);
		glVertex2f(0.2f, 1.5f);
		glVertex2f(0.15f, 1.9f);
	}
	glEnd();

	// Panel lines on fuselage for detail
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		// Horizontal panel lines
		glVertex2f(-0.25f, 1.0f);
		glVertex2f(0.25f, 1.0f);
		glVertex2f(-0.3f, 0.0f);
		glVertex2f(0.3f, 0.0f);
		glVertex2f(-0.27f, -1.0f);
		glVertex2f(0.27f, -1.0f);
		glVertex2f(-0.25f, -1.5f);
		glVertex2f(0.25f, -1.5f);
		// Vertical centerline
		glVertex2f(0.0f, 2.0f);
		glVertex2f(0.0f, -2.5f);
	}
	glEnd();
	glLineWidth(1.0f);

	// COCKPIT CANOPY (more detailed with frame)
	// Canopy glass
	glBegin(GL_POLYGON);
	{
		glColor3fv(cockpitColor);
		glVertex2f(-0.08f, 0.7f);
		glVertex2f(-0.12f, 0.4f);
		glVertex2f(-0.15f, 0.0f);
		glVertex2f(-0.12f, -0.4f);
		glVertex2f(-0.08f, -0.6f);
		glVertex2f(0.08f, -0.6f);
		glVertex2f(0.12f, -0.4f);
		glVertex2f(0.15f, 0.0f);
		glVertex2f(0.12f, 0.4f);
		glVertex2f(0.08f, 0.7f);
	}
	glEnd();

	// Canopy frame
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	{
		glColor3fv(canopyFrame);
		glVertex2f(-0.08f, 0.7f);
		glVertex2f(-0.12f, 0.4f);
		glVertex2f(-0.15f, 0.0f);
		glVertex2f(-0.12f, -0.4f);
		glVertex2f(-0.08f, -0.6f);
		glVertex2f(0.08f, -0.6f);
		glVertex2f(0.12f, -0.4f);
		glVertex2f(0.15f, 0.0f);
		glVertex2f(0.12f, 0.4f);
		glVertex2f(0.08f, 0.7f);
	}
	glEnd();

	// Canopy cross frames
	glBegin(GL_LINES);
	{
		glColor3fv(canopyFrame);
		glVertex2f(0.0f, 0.7f);
		glVertex2f(0.0f, -0.6f);
		glVertex2f(-0.12f, 0.0f);
		glVertex2f(0.12f, 0.0f);
	}
	glEnd();
	glLineWidth(1.0f);

	// MAIN WINGS - Left wing with more detail
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafGrey);
		glVertex2f(-0.3f, 0.1f);  // Wing root leading edge
		glVertex2f(-0.6f, 0.0f);  // Leading edge curve
		glVertex2f(-1.2f, -0.2f); // Leading edge
		glVertex2f(-1.8f, -0.6f); // Wing tip
		glVertex2f(-1.6f, -0.8f); // Trailing edge corner
		glVertex2f(-1.2f, -1.0f); // Trailing edge
		glVertex2f(-0.4f, -0.7f); // Wing root trailing edge curve
		glVertex2f(-0.3f, -0.6f); // Wing root trailing
	}
	glEnd();

	// Wing panel lines - left
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		glVertex2f(-0.8f, -0.1f);
		glVertex2f(-0.8f, -0.8f); // Panel line
		glVertex2f(-1.4f, -0.4f);
		glVertex2f(-1.4f, -0.9f); // Panel line
		// Control surface separation lines
		glVertex2f(-0.5f, -0.65f);
		glVertex2f(-1.35f, -0.95f);
	}
	glEnd();

	// MAIN WINGS - Right wing with more detail
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafGrey);
		glVertex2f(0.3f, 0.1f);	 // Wing root leading edge
		glVertex2f(0.6f, 0.0f);	 // Leading edge curve
		glVertex2f(1.2f, -0.2f); // Leading edge
		glVertex2f(1.8f, -0.6f); // Wing tip
		glVertex2f(1.6f, -0.8f); // Trailing edge corner
		glVertex2f(1.2f, -1.0f); // Trailing edge
		glVertex2f(0.4f, -0.7f); // Wing root trailing edge curve
		glVertex2f(0.3f, -0.6f); // Wing root trailing
	}
	glEnd();

	// Wing panel lines - right
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		glVertex2f(0.8f, -0.1f);
		glVertex2f(0.8f, -0.8f); // Panel line
		glVertex2f(1.4f, -0.4f);
		glVertex2f(1.4f, -0.9f); // Panel line
		// Control surface separation lines
		glVertex2f(0.5f, -0.65f);
		glVertex2f(1.35f, -0.95f);
	}
	glEnd();
	glLineWidth(1.0f);

	// AIR INTAKES - Left
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafDarkGrey);
		glVertex2f(-0.25f, 0.7f);
		glVertex2f(-0.45f, 0.5f);
		glVertex2f(-0.45f, -0.5f);
		glVertex2f(-0.25f, -0.7f);
	}
	glEnd();

	// Intake details - Left
	glBegin(GL_POLYGON);
	{
		glColor3fv(blackColor); // Dark engine intake interior
		glVertex2f(-0.3f, 0.5f);
		glVertex2f(-0.4f, 0.4f);
		glVertex2f(-0.4f, -0.4f);
		glVertex2f(-0.3f, -0.5f);
	}
	glEnd();

	// AIR INTAKES - Right
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafDarkGrey);
		glVertex2f(0.25f, 0.7f);
		glVertex2f(0.45f, 0.5f);
		glVertex2f(0.45f, -0.5f);
		glVertex2f(0.25f, -0.7f);
	}
	glEnd();

	// Intake details - Right
	glBegin(GL_POLYGON);
	{
		glColor3fv(blackColor); // Dark engine intake interior
		glVertex2f(0.3f, 0.5f);
		glVertex2f(0.4f, 0.4f);
		glVertex2f(0.4f, -0.4f);
		glVertex2f(0.3f, -0.5f);
	}
	glEnd();

	// HORIZONTAL STABILIZERS (TAIL WINGS) - Left with better shape
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafDarkGrey);
		glVertex2f(-0.2f, -1.8f);
		glVertex2f(-0.4f, -2.0f); // Improved curve
		glVertex2f(-0.8f, -2.1f); // Improved shape
		glVertex2f(-1.0f, -2.2f);
		glVertex2f(-0.9f, -2.35f); // Better trailing edge
		glVertex2f(-0.8f, -2.5f);
		glVertex2f(-0.4f, -2.4f); // Better curve
		glVertex2f(-0.2f, -2.3f);
	}
	glEnd();

	// Left horizontal stabilizer panel lines
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		glVertex2f(-0.6f, -2.15f);
		glVertex2f(-0.6f, -2.4f); // Panel line
		// Control surface separation
		glVertex2f(-0.3f, -2.25f);
		glVertex2f(-0.85f, -2.4f);
	}
	glEnd();

	// HORIZONTAL STABILIZERS (TAIL WINGS) - Right with better shape
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafDarkGrey);
		glVertex2f(0.2f, -1.8f);
		glVertex2f(0.4f, -2.0f); // Improved curve
		glVertex2f(0.8f, -2.1f); // Improved shape
		glVertex2f(1.0f, -2.2f);
		glVertex2f(0.9f, -2.35f); // Better trailing edge
		glVertex2f(0.8f, -2.5f);
		glVertex2f(0.4f, -2.4f); // Better curve
		glVertex2f(0.2f, -2.3f);
	}
	glEnd();

	// Right horizontal stabilizer panel lines
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		glVertex2f(0.6f, -2.15f);
		glVertex2f(0.6f, -2.4f); // Panel line
		// Control surface separation
		glVertex2f(0.3f, -2.25f);
		glVertex2f(0.85f, -2.4f);
	}
	glEnd();
	glLineWidth(1.0f);

	// VERTICAL STABILIZER (more detailed from top view)
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafDarkGrey);
		glVertex2f(-0.05f, -2.1f);
		glVertex2f(-0.03f, -2.4f);
		glVertex2f(0.0f, -2.7f);
		glVertex2f(0.03f, -2.4f);
		glVertex2f(0.05f, -2.1f);
	}
	glEnd();

	// ENGINE EXHAUSTS (more detailed with gradients)
	glBegin(GL_QUADS);
	{
		glColor3fv(iafDarkGrey);
		glVertex2f(-0.18f, -2.5f);
		glVertex2f(-0.18f, -2.8f);
		glVertex2f(-0.05f, -2.8f);
		glVertex2f(-0.05f, -2.5f);

		glVertex2f(0.05f, -2.5f);
		glVertex2f(0.05f, -2.8f);
		glVertex2f(0.18f, -2.8f);
		glVertex2f(0.18f, -2.5f);
	}
	glEnd();

	// Engine interior - metallic look
	glBegin(GL_QUADS);
	{
		glColor3fv(blackColor);
		glVertex2f(-0.16f, -2.55f);
		glVertex2f(-0.16f, -2.75f);
		glVertex2f(-0.07f, -2.75f);
		glVertex2f(-0.07f, -2.55f);

		glVertex2f(0.07f, -2.55f);
		glVertex2f(0.07f, -2.75f);
		glVertex2f(0.16f, -2.75f);
		glVertex2f(0.16f, -2.55f);
	}
	glEnd();

	// WING-MOUNTED WEAPONS (MISSILES) - Left wing (more detailed)
	// Missile body
	glBegin(GL_POLYGON);
	{
		glColor3fv(missileColor);
		glVertex2f(-1.2f, -0.4f);
		glVertex2f(-1.25f, -0.4f);
		glVertex2f(-1.28f, -0.45f);
		glVertex2f(-1.3f, -0.5f);
		glVertex2f(-1.3f, -0.9f);
		glVertex2f(-1.28f, -0.95f);
		glVertex2f(-1.25f, -1.0f);
		glVertex2f(-1.2f, -1.0f);
	}
	glEnd();

	// Missile details
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		glVertex2f(-1.25f, -0.5f);
		glVertex2f(-1.25f, -0.9f); // Panel line
	}
	glEnd();

	// Missile fins
	glBegin(GL_TRIANGLES);
	{
		glColor3fv(missileDetailColor);
		// Rear fins
		glVertex2f(-1.2f, -0.95f);
		glVertex2f(-1.1f, -1.05f);
		glVertex2f(-1.25f, -0.85f);

		glVertex2f(-1.3f, -0.85f);
		glVertex2f(-1.4f, -1.05f);
		glVertex2f(-1.3f, -0.95f);
	}
	glEnd();

	// WING-MOUNTED WEAPONS (MISSILES) - Right wing (more detailed)
	// Missile body
	glBegin(GL_POLYGON);
	{
		glColor3fv(missileColor);
		glVertex2f(1.2f, -0.4f);
		glVertex2f(1.25f, -0.4f);
		glVertex2f(1.28f, -0.45f);
		glVertex2f(1.3f, -0.5f);
		glVertex2f(1.3f, -0.9f);
		glVertex2f(1.28f, -0.95f);
		glVertex2f(1.25f, -1.0f);
		glVertex2f(1.2f, -1.0f);
	}
	glEnd();

	// Missile details
	glBegin(GL_LINES);
	{
		glColor3fv(blackColor);
		glVertex2f(1.25f, -0.5f);
		glVertex2f(1.25f, -0.9f); // Panel line
	}
	glEnd();

	// Missile fins
	glBegin(GL_TRIANGLES);
	{
		glColor3fv(missileDetailColor);
		// Rear fins
		glVertex2f(1.2f, -0.95f);
		glVertex2f(1.1f, -1.05f);
		glVertex2f(1.25f, -0.85f);

		glVertex2f(1.3f, -0.85f);
		glVertex2f(1.4f, -1.05f);
		glVertex2f(1.3f, -0.95f);
	}
	glEnd();
	glLineWidth(1.0f);

	// NAVIGATION LIGHTS
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	{
		glColor3fv(navLightRed);
		glVertex2f(-1.8f, -0.6f); // Left wing tip

		glColor3fv(navLightGreen);
		glVertex2f(1.8f, -0.6f); // Right wing tip
	}
	glEnd();
	glPointSize(1.0f);

	// IAF ROUNDELS on wings
	DrawIAFRoundel(0.9f, -0.45f, 0.15f);  // Right wing roundel
	DrawIAFRoundel(-0.9f, -0.45f, 0.15f); // Left wing roundel

	// ADDITIONAL DETAILS - Fuel tanks under wings
	glBegin(GL_POLYGON);
	{
		glColor3fv(iafLightGrey);
		glVertex2f(-0.6f, -0.4f);
		glVertex2f(-0.65f, -0.45f);
		glVertex2f(-0.65f, -0.8f);
		glVertex2f(-0.6f, -0.85f);
		glVertex2f(-0.55f, -0.8f);
		glVertex2f(-0.55f, -0.45f);
	}
	glEnd();

	glBegin(GL_POLYGON);
	{
		glColor3fv(iafLightGrey);
		glVertex2f(0.6f, -0.4f);
		glVertex2f(0.65f, -0.45f);
		glVertex2f(0.65f, -0.8f);
		glVertex2f(0.6f, -0.85f);
		glVertex2f(0.55f, -0.8f);
		glVertex2f(0.55f, -0.45f);
	}
	glEnd();

	// Reset rotation to original
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
}
