#ifndef SCENE_LIGHT_H
#define SCENE_LIGHT_H

#include "Utils.h"

/*
SceneLight Class - The light properties of a single light-source in a ray-trace scene

The Scene class holds a list of these
*/
class SceneLight
{
public:
  float attenuationConstant, attenuationLinear, attenuationQuadratic;
  Vector color;
  Vector position;
  Vector ambiant;
  Vector diffuse;
  Vector specular;
};



#endif