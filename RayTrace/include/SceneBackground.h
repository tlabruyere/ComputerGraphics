/**
 * Author: Tom LaBruyere
 * file: SceneBackground.h
 */
#ifndef SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEBACKGROUND_H__
#define SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEBACKGROUND_H__

/**
 * SceneBackground Class - The Background properties of a ray-trace scene
 * This class defines the background in the Scene
 */
class SceneBackground
{
    public:
        Vector color;
        Vector ambientLight;
};

#endif
