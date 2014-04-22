#ifndef SCENE_MATERIAL_H
#define SCENE_MATERIAL_H

#include "Utils.h"

/*
SceneMaterial Class - The material properties used in a ray-trace scene

The Scene class holds a list of material
*/
class SceneMaterial
{
public:
  std::string name;
  std::string texture;
  Vector diffuse;
  Vector specular;
  float shininess;
  Vector transparent;
  Vector reflective;
  Vector refraction_index;

  // -- Constructors & Destructors --
  SceneMaterial (void)
  {}

  ~SceneMaterial (void)
  {}
};


#endif // SCENE_MATERIAL_H
