/**
 * NOTE: You do not need to edit this file for this assignment but may do so
 * 
 * This file defines the following:
 * NormalRenderer Class
 *
 * This is the "Normal Renderer" which draws the descriptor scene with normal
 * OpenGL calls in real-time rather than ray-tracing
 * This can be a good reference in learning to access the scene objects
 */

#ifndef NORMALRENDERER_H
#define NORMALRENDERER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Scene.h"
#include "Utils.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#endif
/**
 *  NormalRenderer Class - The class that will render the scene as normal objects in real-time
 *   This class renders the scene when the RayTrace class isn't rendering
 */
class NormalRenderer
{
    public:
        // -- Constructors & Destructors --
        NormalRenderer (void) : m_pScene (NULL) 
        {}
        ~NormalRenderer (void) 
        {}

        // - SetScene - Sets the scene variable for the Renderer to draw the scene
        void SetScene(Scene *scn) { m_pScene = scn; }

        // - RenderScene - Draws the scene with OpenGL calls
        void RenderScene();
    
    private:
        // - Scene Variable Pointer for the Scene Definition - 
        Scene *m_pScene;

        // - SetRenderStates - Sets the OpenGL Render States
        void SetRenderStates();

        // - UnsetRenderStates - Sets the OpenGL Render States Back
        void UnsetRenderStates();
};

#endif // NORMALRENDERER_H
