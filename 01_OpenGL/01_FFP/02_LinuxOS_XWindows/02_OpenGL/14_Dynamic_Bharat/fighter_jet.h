/**************************************************
 * S.G.M.P
 * @file:
 * @brief:
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @copyright ©2020-2025 MarshallingVoid
 * @date:
 **************************************************/

#ifndef FIGHTER_JET_H
#define FIGHTER_JET_H

/**
 * @brief Draws a 2D representation of an Indian Air Force fighter jet using OpenGL.
 *
 * This function uses OpenGL primitives to create a simple representation of an IAF fighter jet.
 *
 */
void DrawIAFJet(GLfloat smoke_red, GLfloat smoke_green, GLfloat smoke_blue);

/**
 * @brief Initializes the smoke particle system.
 *
 * This function sets up the initial state of the smoke particles used in the fighter jet animation.
 */
void InitSmokeParticles();

#endif /* FIGHTER_JET_H */