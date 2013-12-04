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
#include <vector>
#include <string>

/*
RayTrace Class - The class containing the function you will need to implement

This is the class with the function you need to implement
*/


class RayTrace
{
private:
  static const int TRACE_DEPTH = 10;

  //Ray ComputeStartRay( int screenX, int screenY)
  Ray ComputeStartRay( float screenX, float screenY)
  {
    Vector lookVector = (m_Scene[curScene]->GetCamera().GetTarget() - m_Scene[curScene]->GetCamera().position).Normalize(); 
    Vector camera_left = m_Scene[curScene]->GetCamera().GetUp().Cross(lookVector).Normalize() ;
    Vector camera_right = lookVector.Cross(m_Scene[curScene]->GetCamera().GetUp()).Normalize() ;

    float H = 2.0 * m_Scene[curScene]->GetCamera().GetNearClip()
      * tan((m_Scene[curScene]->GetCamera().GetFOV()/2.0)
      *(M_PI/180));// + deltaScreenY;
    float W = H * ((float)WINDOW_WIDTH/WINDOW_HEIGHT);

    float deltaScreenX = (W/WINDOW_WIDTH) * screenX;
    float deltaScreenY = (H/WINDOW_HEIGHT) * screenY;

    float WOver2 = (deltaScreenX - W/2.0 );
    float HOver2 = (deltaScreenY - H/2.0 );

    Vector centerOfImagePlane = m_Scene[curScene]->GetCamera().GetPosition() 
      + lookVector * m_Scene[curScene]->GetCamera().GetNearClip();

    Vector image_point = centerOfImagePlane 
      + (m_Scene[curScene]->GetCamera().GetUp() * HOver2)
      - (camera_left * WOver2);

    Vector ray_direction = image_point -  m_Scene[curScene]->GetCamera().position;
    return Ray(m_Scene[curScene]->GetCamera().position, ray_direction, image_point);
  }

  Vector Trace(Ray pRay, int pDepth, float pDist )
  {
    if(pDepth > TRACE_DEPTH)
    {
      return m_Scene[curScene]->GetBackground().color;
    }
    int closestObj = -1;
    float closestDist = FLT_MAX;
    for(int i = 0; i<m_Scene[curScene]->GetNumObjects(); i++)
    {
      float t = m_Scene[curScene]->getObject(i)->IntersectionTest(pRay);
      if( t>=0.0 && t < closestDist)
      {
        closestDist = t;
        closestObj = i;
      }
    }
    if(closestObj >= 0) //intersection
    {
      Vector color = AccLightSource(pRay, closestObj, closestDist );
      color = color+ ComputeReflection(pRay, closestObj, closestDist, pDepth);
      color = color+ ComputeRefraction(pRay, closestObj, closestDist, pDepth);
      return  color;
    }
    else // no intersection off to infinity
    {
      Vector color = m_Scene[curScene]->GetBackground().color;
      return color;
    }
  
  }

  Vector AccLightSource(Ray pRay, int pObjIdx, float pIntersectionTime)
  {
    Vector color;//= m_Scene[curScene]->GetBackground().ambientLight;
    for(int lightIdx = 0; lightIdx< m_Scene[curScene]->GetNumLights();lightIdx++)
    {
      Vector surfPt =  pRay.GetPoint(pIntersectionTime);
      bool objIntersection = false;
      for(int objIdx = 0; objIdx < m_Scene[curScene]->GetNumObjects(); objIdx++)
      {
        Ray point2Light(pRay.GetPoint(pIntersectionTime)+m_Scene[curScene]->getObject(objIdx)->GetNormal(pRay, pIntersectionTime)*Tools::EPSILON,
          m_Scene[curScene]->GetLight(lightIdx).position - pRay.GetPoint(pIntersectionTime)); 
        float t = m_Scene[curScene]->getObject(objIdx)->IntersectionTest( point2Light);
        if(t>0) // if t is found, then there is an object blocking the light
        {
          objIntersection = true;
          break;
        }
      }
      if(!objIntersection)
      {
        color = color + m_Scene[curScene]->getObject(pObjIdx)->GetColor(m_Scene[curScene]->GetLight(lightIdx), pRay, pIntersectionTime) /m_Scene[curScene]->GetNumLights();
      }
    }
    return color;
  }
   
  Vector ComputeReflection(Ray pRay, int pObjIdx, float pIntersectionTime, int traceDepth)
  {
    Vector color(0.0,0.0,0.0);
    Vector objReflectivity = m_Scene[curScene]->getObject(pObjIdx)->GetReflectivity(pRay, pIntersectionTime);
    if(objReflectivity.Magnitude() > 0.0)
    {
      Vector surfPt = pRay.GetPoint(pIntersectionTime);
      Vector surfPtNormal = m_Scene[curScene]->getObject(pObjIdx)->GetNormal(pRay, pIntersectionTime);
      Vector reflectionDir = Tools::Reflection( pRay.GetDirection(), surfPtNormal);
      Ray Reflection(surfPt + surfPtNormal*Tools::EPSILON, reflectionDir);
      Vector reflectionColor = Trace(Reflection, traceDepth + 1, pIntersectionTime);//pDist + closestDist);
      color =  reflectionColor*m_Scene[curScene]->getObject(pObjIdx)->GetReflectivity(pRay, pIntersectionTime);
    }
    return color;
  }

  Vector ComputeRefraction(Ray pRay, int pObjIdx, float pIntersectionTime, int traceDepth)
  {
    Vector color(0.0,0.0,0.0);
/*    Vector objRefractivity = m_Scene[curScene]->getObject(pObjIdx)->GetRefractivity(pRay, pIntersectionTime);
//    if(objRefractivity.Magnitude() > 0.0)
    if(objRefractivity.x > 0.0)
    {
      Vector surfPt = pRay.GetPoint(pIntersectionTime);
      Vector surfPtNormal = m_Scene[curScene]->getObject(pObjIdx)->GetNormal(pRay, pIntersectionTime);
      // assume always going either from air to material or material to air. For easyness
      float rayDotNormal = pRay.GetDirection().Dot(surfPtNormal);
      float ang = acos(rayDotNormal);
      // compute coeff
      float snellN1N2;
      if(abs(ang) < M_PI)//entering material
      {
        snellN1N2 = 1.0/objRefractivity.x;
      }
      else // leaving material
      {
        snellN1N2 = objRefractivity.x;
	// normal need to be inword
	surfPtNormal = surfPtNormal*-1.0;
      }

      // give me transmission vector
      Vector trans = pRay.GetDirection()*snellN1N2 
        - surfPtNormal * (snellN1N2 * rayDotNormal - sqrt(1-((snellN1N2*snellN1N2)*(sin(rayDotNormal)*sin(rayDotNormal)))));
      Ray Refraction(surfPt + surfPtNormal*Tools::EPSILON * -1.0, trans);
      Vector refractionColor = Trace(Refraction, traceDepth + 1, pIntersectionTime);
      color = refractionColor;
    }
    */
     Vector objRefractivity = m_Scene[curScene]->getObject(pObjIdx)->GetRefractivity(pRay, pIntersectionTime);
    if(objRefractivity.x > 0.0)
    {
      Vector surfPt = pRay.GetPoint(pIntersectionTime);
      Vector surfPtNormal = m_Scene[curScene]->getObject(pObjIdx)->GetNormal(pRay, pIntersectionTime) * result(pRay, pObjIdx, surfPt);
      // compute coeff
      float snellN1N2 = 1.0/objRefractivity.x;

      float cosl = -1.0* surfPtNormal.Dot(pRay.GetDirection());
      float cosT2 = 1.0 - snellN1N2 * snellN1N2 *(1.0 - cosl *cosl);
      if(cosT2>0.0)
      {
        Vector trans = pRay.GetDirection()*snellN1N2 + surfPtNormal* (snellN1N2 * cosl - sqrt(cosT2));
        Ray Refraction(surfPt + trans*Tools::EPSILON , trans);
        Vector refractionColor = Trace(Refraction, traceDepth + 1, pIntersectionTime);
        color = refractionColor;
      }
    }
    return color;

  }

  float result(Ray pRay, int objIdx, Vector pSurfPt)
  {
    Ray oppDir(pSurfPt+pRay.GetDirection()*Tools::EPSILON, pRay.GetDirection()*-1.0);

    float retVal;
    float t = m_Scene[curScene]->getObject(objIdx)->IntersectionTest(oppDir);
    if(t>0) //  self intersection
      retVal = -1.0;
    else 
      retVal = 1.0;
    return retVal;
  }


public:
  //- Scene Variable for the Scene Definition - 
  std::vector<Scene*> m_Scene;
  int curScene;

  // -- Constructors & Destructors --
  RayTrace (void) {curScene = 0;}
  ~RayTrace (void) 
  {
    for(int i =0;i<m_Scene.size();i++)
    {
      if(m_Scene[i] != nullptr)
	delete m_Scene[i];
      m_Scene[i] = nullptr;
    }
  }

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
      through its functions such as m_Scene[curScene]->GetBackground (), m_Scene[curScene]->GetNumLights (), 
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

/*    Vector retVecPortion[5];
    retVecPortion[0] =  Trace(ComputeStartRay(screenX, screenY), 0, 0.0);
    retVecPortion[1] =  Trace(ComputeStartRay(screenX+1, screenY), 0, 0.0);
    retVecPortion[2] =  Trace(ComputeStartRay(screenX, screenY+1), 0, 0.0);
    retVecPortion[3] =  Trace(ComputeStartRay(screenX+1, screenY+1), 0, 0.0);
    retVecPortion[4] =  Trace(ComputeStartRay(screenX+0.5, screenY+0.5), 0, 0.0);

    Vector retVec = (retVecPortion[0] + retVecPortion[1] + retVecPortion[2] + retVecPortion[3] + retVecPortion[4] )/ 5;
    */
    Vector retVec = Trace(ComputeStartRay(screenX, screenY), 0, 0.0);



    // clamp if necessary ( may not be needed)
    if(retVec.x >1.0)
    {
      retVec.x = 1.0;
    }
    if(retVec.y >1.0)
    {
      retVec.y = 1.0;
    }
    if(retVec.z >1.0)
    {
      retVec.z = 1.0;
    }
    return retVec;
  }

  void incrementScene()
  {
    if(curScene +1 < m_Scene.size())
    {
      curScene++;
    }
    else
      curScene = 0;
  }

  bool LoadScene( std::string xmlFile)
  {
    Scene* scene = new Scene;
    bool loaded = scene->Load((char*)xmlFile.c_str());
    m_Scene.push_back(scene);
    return loaded;
  }

  Scene* GetCurScene()
  {
    return m_Scene[curScene];
  }
};

#endif // RAYTRACE_H
