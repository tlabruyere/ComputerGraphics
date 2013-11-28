#ifndef TOOLS_H
#define TOOLS_H
#include <algorithm>
#include <cmath>

#include "Utils.h"
#include "SceneMaterial.h"
#include "SceneLight.h"

class Tools
{
public:
  static Vector ParseColor (XMLNode node)
  {
    if (node.isEmpty ())
      return Vector (0.0f, 0.0f, 0.0f);
    return Vector (atof(node.getAttribute("red")), 
      atof(node.getAttribute("green")),
      atof(node.getAttribute("blue")));
  }
  
  static Vector GetPhongColor(SceneMaterial pMat, SceneLight pLight, Vector pNormal, Vector pReflection, Vector pLightVec, Vector pLookVec)
  {
    Vector diffuse = Diffuse(pMat.diffuse, pLight.color, pLightVec, pNormal);
    Vector specular = Specular(pMat.specular, pMat.shininess, pLight.color, pLookVec, pReflection);
    return diffuse + specular;

  }

  static Vector Reflection(Vector pLookDir, Vector pNormal)
  {
    return (pNormal 
      * 2.0 
      * (max(pLookDir.Dot(pNormal),0.0)) 
      - pLookDir).Normalize();
  }

  static Vector Diffuse(Vector pMatDiffuse, Vector pLightColor, Vector pLightDir, Vector pNormal)
  {
    return pMatDiffuse * pLightColor * max( pLightDir.Dot(pNormal), 0.0);
  }
  
  static Vector Specular(Vector pMatSpec, float pMatShine, Vector pLightColor, Vector pLookDir, Vector pReflection)
  {
    return pMatSpec * pLightColor * pow(max(pLookDir.Dot(pReflection),0.0), pMatShine);
  }

  static float AreaOfTriangle( Vector p0, Vector p1, Vector p2)
  {
    Vector p1p0 = p1 - p0;
    Vector p2p0 = p2 - p0;
    float theta = acos( ((p1p0).Dot(p2p0)) / (p1p0.Magnitude() * p2p0.Magnitude()) );
    float area = 0.5 * (p1p0.Magnitude() * p2p0.Magnitude()) * sin(theta);
    return area;

  }
};

#endif //TOOLS_H