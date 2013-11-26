/*
NOTE: This is the file you will need to begin working with
You will need to implement the RayTrace::CalculatePixel () function

This file defines the following:
RayTrace Class
*/

#ifndef RAYTRACE_H
#define RAYTRACE_H

#include <stdio.h>
#include <stdlib.h>

#include "Utils.h"
#include "Scene.h"
#include "Ray.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES 
#include <math.h>

/*
RayTrace Class - The class containing the function you will need to implement

This is the class with the function you need to implement
*/


class RayTrace
{
private:
  static const int TRACE_DEPTH = 10;

  Ray ComputeStartRay( int screenX, int screenY)
  {
    Vector lookVector = (m_Scene.GetCamera().GetTarget() - m_Scene.GetCamera().position).Normalize(); 
    Vector camera_left = lookVector.Cross( m_Scene.GetCamera().GetUp()).Normalize() * -1.0;

    float H = 2.0 * m_Scene.GetCamera().GetNearClip()
      * tan((m_Scene.GetCamera().GetFOV()/2.0)
      *(M_PI/180));// + deltaScreenY;
    float W = H * ((float)WINDOW_WIDTH/WINDOW_HEIGHT);

    float deltaScreenX = (W/WINDOW_WIDTH) * screenX;
    float deltaScreenY = (H/WINDOW_HEIGHT) * screenY;

    float WOver2 = (W/2.0  - deltaScreenX);
    float HOver2 = (H/2.0 - deltaScreenY);

    Vector lookat = m_Scene.GetCamera().GetPosition() + lookVector
      * m_Scene.GetCamera().GetNearClip();

    Vector image_point = lookat 
      + (m_Scene.GetCamera().GetUp() * HOver2)
      + (camera_left * WOver2);
    Vector ray_direction = image_point -  m_Scene.GetCamera().position;
    return Ray(m_Scene.GetCamera().position, ray_direction, image_point);
  }

  Vector Trace(Ray pRay, int pDepth )
  {
    Vector color = m_Scene.GetBackground().color;
    if(pDepth > TRACE_DEPTH)
    {
      return color;
    }
    int closestObj = 0;
    float closestDist = FLT_MAX;
    for(int i = 0; i<m_Scene.GetNumObjects(); i++)
    {
      float t = m_Scene.getObject(i)->IntersectionTest(pRay);
      if(t < closestDist)
      {
        closestDist = t;
        closestObj = i;
      }
    }
    if(closestDist < 0.0)
    {
      return color;
    }
    else 
    {
//      return m_Scene.getObject(i)->get
      return Vector(0.0,0.0,0.0);
    }
    

  }

public:
  //- Scene Variable for the Scene Definition - 
  Scene m_Scene;

  // -- Constructors & Destructors --
  RayTrace (void) {}
  ~RayTrace (void) {}

  // -- Main Functions --
  // - CalculatePixel - Returns the Computed Pixel for that screen coordinate
  Vector CalculatePixel (int screenX, int screenY)
  {
 /*
                -- How to Implement a Ray Tracer --

    This computed pixel will take into account the camera and the scene
      and return a Vector of <Red, Green, Blue>, each component ranging from 0.0 to 1.0

    In order to start working on computing the color of this pixel,
      you will need to cast a ray from your current camera position
      to the image-plane at the given screenX and screenY
      coordinates and figure out how/where this ray intersects with 
      the objects in the scene descriptor.
      The Scene Class exposes all of the scene's variables for you 
      through its functions such as m_Scene.GetBackground (), m_Scene.GetNumLights (), 
      etc. so you will need to use those to learn about the World.

    To determine if your ray intersects with an object in the scene, 
      you must calculate where your objects are in 3D-space [using 
      the object's scale, rotation, and position is extra credit]
      and mathematically solving for an intersection point corresponding to that object.

    For example, for each of the spheres in the scene, you can use 
      the equation of a sphere/ellipsoid to determine whether or not 
      your ray from the camera's position to the screen-pixel intersects 
      with the object, then from the incident angle to the normal at 
      the contact, you can determine the reflected ray, and recursively 
      repeat this process capped by a number of iterations (e.g. 10).

    Using the lighting equation to calculate the color at every 
      intersection point and adding its fractional amount (determined by the material)
      will get you a final color that returns to the eye at this point.
*/

    if ((screenX < 0 || screenX > WINDOW_WIDTH - 1) ||
      (screenY < 0 || screenY > WINDOW_HEIGHT - 1))
    {
      // Off the screen, return black
      return Vector (0.0f, 0.0f, 0.0f);
    }

    // Until this function is implemented, return white
//    return Vector (1.0f, 1.0f, 1.0f);
    Vector retVec =  Trace(ComputeStartRay(screenX, screenY),1);
    return retVec;
  }
};

#endif // RAYTRACE_H
