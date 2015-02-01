/*
NOTE: You do not need to edit this file for this assignment but may do so

This file defines the following:
SceneObjectType Enumeration
SceneBackground Class
SceneLight Class
SceneMaterial Class
SceneObject Class -> SceneSphere, SceneTriangle, SceneModel
Scene Class

Scene Usage: Several Functions
Scene::Load (sceneFile) - load a scene descriptor file

Scene::GetDescription () - get the scene description string
Scene::GetAuthor () - get the scene author string
Scene::GetBackground () - get the scene background information
Scene::GetNumLights () - get the number of lights in the scene
Scene::GetLight (lightIndex) - get one of the lights in the scene
Scene::GetNumMaterials () - get the number of materials in the scene
Scene::GetMaterial (materialIndex OR materialName) - get a material's description
Scene::GetNumObjects () - get the number of objects in the scene
Scene::GetObject (objectIndex) - get an object's description
Scene::GetCamera - get the current camera description

The trickiest one of these is the GetObject () function
It is used like this:
[The object is type-casted to its corresponding type]

SceneObject tempObject = m_Scene.GetObject (0);
if (tempObject.IsTriangle ())
{
Vector vertices[3];

for (int n = 0; n < 3; n++)
{
vertices[n] = ((SceneTriangle)tempObject).vertex[n];
}
}
*/

#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "Utils.h"
#include "Ray.h"
#include "SceneMaterial.h"
#include "SceneMaterialMgr.h"
#include "Texture.h"

// XML Parser by Frank Vanden Berghen
// Available: http://iridia.ulb.ac.be/~fvandenb/tools/xmlParser.html
#include "xmlParser.h"

// 3DS File by bkenwright@xbdev.net
//    Updated by Raphael Mun
#include "3ds.h"
#include <algorithm>

// Max Line Length for OBJ File Loading
#define MAX_LINE_LEN 1000

#define CHECK_ATTR(a) (a == NULL ? "" : a)
#define CHECK_ATTR2(a,b) (a == NULL ? b : a)

/*
SceneObjectType Namespace - Holds the ObjectType Enumeration
*/
namespace SceneObjectType
{
  enum ObjectType
  {
    Sphere = 0,
    Triangle = 1,
    Model = 2,
  };
};

/*
SceneBackground Class - The Background properties of a ray-trace scene

This class defines the background in the Scene
*/
class SceneBackground
{
public:
  Vector color;
  Vector ambientLight;
};


/*
SceneObject Class - A base object class that defines the common features of all objects

This is the base object class that the various scene object types derive from
*/
class SceneObject
{
public:
  std::string name;
  SceneObjectType::ObjectType type;
  Vector scale, rotation, position;

  // -- Constructors & Destructors --
  SceneObject (void) { scale.x = 1.0f; scale.y = 1.0f; scale.z = 1.0f; }
  SceneObject (SceneObjectType::ObjectType tp) : type (tp) { scale.x = 1.0f; scale.y = 1.0f; scale.z = 1.0f; }
  SceneObject (std::string nm, SceneObjectType::ObjectType tp) : name(nm), type (tp) { scale.x = 1.0f; scale.y = 1.0f; scale.z = 1.0f; }

  // -- Object Type Checking Functions --
  bool IsSphere (void) { return (type == SceneObjectType::Sphere); }
  bool IsTriangle (void) { return (type == SceneObjectType::Triangle); }
  bool IsModel (void) { return (type == SceneObjectType::Model); }

  /* 
   * Tests for intersection between Ray and object
   * If intersection returns the t value in the parametric equation P(x) = Po + t*Pd so the point intersection can be computed
   * else return a negitive number classifying there is no intersection in the direction of the ray
   */
  virtual float IntersectionTest(Ray pRay) = 0;
  virtual Vector GetColor(SceneLight pLight, Ray lookVector, float pDist)
  {
    return Vector(0.0,0.0,0.0);
  }

  virtual Vector GetNormal(Ray pRay, float intersectionTime)
  {
    return Vector(0.0,0.0,0.0);
  }
  
  virtual Vector GetReflectivity(Ray pRay, float intersectionTime)
  {
    return Vector(0.0,0.0,0.0);
  }
  
  virtual Vector GetRefractivity(Ray pRay, float intersectionTime) =0;
};

/*
SceneSphere Class - The sphere scene object

Sphere object derived from the SceneObject
*/
class SceneSphere : public SceneObject
{
public:
  std::string material;
  Vector center;
  float radius;

  // -- Constructors & Destructors --
  SceneSphere (void) : SceneObject ("Sphere", SceneObjectType::Sphere) {}
  SceneSphere (std::string nm) : SceneObject (nm, SceneObjectType::Sphere) {}

  float IntersectionTest(Ray pRay)
  {
    float t_return = -1.0;
    //Vector sphereCenMinusRayOrg = center - pRay.GetOrigin();
    Vector sphereCenMinusRayOrg = pRay.GetOrigin() - center;
    float A = pRay.GetDirection().Dot( pRay.GetDirection());
    float B = (sphereCenMinusRayOrg ).Dot(pRay.GetDirection() * 2.0);
    float C = (sphereCenMinusRayOrg).Dot(sphereCenMinusRayOrg) - (radius*radius);
    
    float imaginaryTest = B*B - 4.0 * A * C;
    if(imaginaryTest > 0.0) // if imaginary, there is no value where the ray intersects the sphere
    {
      float t_minus = (-B - sqrt(B*B - 4.0 * A * C)) / 2*A; 
      t_return =  (-B + sqrt(B*B - 4.0 * A * C)) / 2*A; 
      if( t_minus >= 0.0 && t_minus < t_return)
        t_return = t_minus;
    } 
    return t_return;
  }

  Vector GetColor(SceneLight pLight, Ray lookVector, float pDist)
  {
    Vector surfPt = lookVector.GetPoint(pDist);
    Vector ptNormal = GetNormal(lookVector, pDist);
    Vector lightVector = (pLight.position-surfPt).Normalize();
    Vector ReflectionVec = Tools::Reflection(lightVector,ptNormal); // (ptNormal * 2.0 * (max(lookVector.GetDirection().Dot(ptNormal),0.0)) - lookVector.GetDirection()).Normalize();
    SceneMaterialMgr& materialMgr = SceneMaterialMgr::GetInstance();

    return Tools::GetPhongColor(
      materialMgr.GetMaterial( material), 
      pLight, 
      ptNormal, 
      Tools::Reflection(lookVector.GetDirection(),ptNormal),
      lightVector,
      lookVector.GetDirection(),
      pDist);


  }
 
  Vector GetNormal(Ray pRay, float intersectionTime)
  {
    Vector surfPt = pRay.GetPoint(intersectionTime);
    return (surfPt - center).Normalize();
  }

  Vector GetReflectivity(Ray pRay, float intersectionTime)
  {
    SceneMaterialMgr &matMgr = SceneMaterialMgr::GetInstance();
    return matMgr.GetMaterial(material).reflective;
  }

  Vector GetRefractivity(Ray pRay, float intersectionTime)
  {
    SceneMaterialMgr &matMgr = SceneMaterialMgr::GetInstance();
    return matMgr.GetMaterial(material).refraction_index;
  }
};

/*
SceneTriangle Class - The triangle scene object

Single triangle object derived from the SceneObject
*/
class SceneTriangle : public SceneObject
{
private:
  Vector BarycentricCooef(Vector point)
  {
    Vector coord(0.0,0.0,0.0); // alpha =x, beta = y, gamma = z
      float totalAreaOfTriangle = Tools::AreaOfTriangle( vertex[0],vertex[1],vertex[2]);

      coord.x = Tools::AreaOfTriangle( point,vertex[1],vertex[2])/totalAreaOfTriangle;
      coord.y = Tools::AreaOfTriangle( vertex[0], point,vertex[2])/totalAreaOfTriangle;
      coord.z = Tools::AreaOfTriangle( vertex[0],vertex[1], point)/totalAreaOfTriangle;

//      float pointArea = coord.x + coord.y + coord.z;
      return coord;
  }

public:
  std::string material[3];
  Vector vertex[3];
  Vector normal[3];
  float u[3], v[3];

  // -- Constructors & Destructors --
  SceneTriangle (void) : SceneObject ("Triangle", SceneObjectType::Triangle) {}
  SceneTriangle (std::string nm) : SceneObject (nm, SceneObjectType::Triangle) {}
  float IntersectionTest(Ray pRay)
  {
    float t = -1.0;
    // Get normal for plane
    Vector planeNormal = GetNormal( pRay, 0.0);
    Vector lineFrmPtOnPlane2rayOrg = (vertex[0] - pRay.GetOrigin());
    
    float t_potential = (planeNormal.Dot(lineFrmPtOnPlane2rayOrg)) / (planeNormal.Dot( pRay.GetDirection()));
    if(t_potential > Tools::EPSILON)
    {
      //Compute whether point is within the bounds of the triangle
      Vector pointOfIntersection = pRay.GetPoint(t_potential);

      float totalAreaOfTriangle = Tools::AreaOfTriangle( vertex[0],vertex[1],vertex[2]);

      float ptTriangle0 = Tools::AreaOfTriangle( pointOfIntersection,vertex[1],vertex[2]);
      float ptTriangle1 = Tools::AreaOfTriangle( vertex[0], pointOfIntersection,vertex[2]);
      float ptTriangle2 = Tools::AreaOfTriangle( vertex[0],vertex[1], pointOfIntersection);

//      Vector coord = BarycentricCooef(pointOfIntersection);

      //float pointArea = coord.x + coord.y + coord.z;
      float pointArea = ptTriangle0 + ptTriangle1 + ptTriangle2;

      if(abs( totalAreaOfTriangle - pointArea) < Tools::EPSILON&& abs( totalAreaOfTriangle - pointArea) > -Tools::EPSILON)
        t = t_potential;
    }
    return t;
  }

  Vector GetColor(SceneLight pLight, Ray lookVector, float pDist)
  {
    Vector outColor(0.0,0.0,0.0);
    Vector planeNormal = GetNormal( lookVector, 0.0);
    Vector surfPt = lookVector.GetPoint(pDist);
    SceneMaterialMgr &matMgr = SceneMaterialMgr::GetInstance();
    SceneMaterial mat = matMgr.GetMaterial(material[0]); // Assumption: if first vertex is texture all will be textured
    if(mat.name.compare("CHECKERBOARD")==0) // return Texture
    {
      float totalAreaOfTriangle = Tools::AreaOfTriangle( vertex[0],vertex[1],vertex[2]);
    
      const int numVertex = 3;
      float ptTriangle[numVertex];
      ptTriangle[0] = Tools::AreaOfTriangle( surfPt, vertex[1], vertex[2])/ totalAreaOfTriangle;
      ptTriangle[1] = Tools::AreaOfTriangle( vertex[0], surfPt, vertex[2])/ totalAreaOfTriangle;
      ptTriangle[2] = Tools::AreaOfTriangle( vertex[0], vertex[1], surfPt)/ totalAreaOfTriangle;

      float curU = u[0]*ptTriangle[0] + u[1]*ptTriangle[1] + u[2]*ptTriangle[2];
      float curV = v[0]*ptTriangle[0] + v[1]*ptTriangle[1] + v[2]*ptTriangle[2];
      outColor = Texture::CheckerBoardTexture(curU,curV);

    }
    else // compute phong
    {
      Vector lightVector = (pLight.position-surfPt).Normalize();
  
      float totalAreaOfTriangle = Tools::AreaOfTriangle( vertex[0],vertex[1],vertex[2]);
    
      const int numVertex = 3;
      float ptTriangle[numVertex];
      ptTriangle[0] = Tools::AreaOfTriangle( surfPt, vertex[1], vertex[2])/ totalAreaOfTriangle;
      ptTriangle[1] = Tools::AreaOfTriangle( vertex[0], surfPt, vertex[2])/ totalAreaOfTriangle;
      ptTriangle[2] = Tools::AreaOfTriangle( vertex[0], vertex[1], surfPt)/ totalAreaOfTriangle;

//      float test = ptTriangle[0] + ptTriangle[1] + ptTriangle[2];
      // compute phong shading per vertex
      Vector vertColor[numVertex];
      for(int i=0; i<numVertex; i++)
      {
        vertColor[i] = Tools::GetPhongColor(
          matMgr.GetMaterial(material[i]),
          pLight,
          planeNormal,// normal[i],
          Tools::Reflection(lookVector.GetDirection(),planeNormal),
          lightVector,
          lookVector.GetDirection(),
          pDist);
        outColor = outColor + vertColor[i]*ptTriangle[i];
      }
    }

    return outColor;
  }
  
  Vector GetNormal(Ray pRay, float intersectionTime)
  {
/*    Vector normalOut(0.0, 0.0, 0.0);
    Vector coord = BarycentricCooef(pSurfPt);
    Vector norm0Ref = normal[0].Normalize();
    Vector norm1Ref = normal[1].Normalize();
    Vector norm2Ref = normal[2].Normalize();

    normalOut = (norm0Ref*coord.x + norm1Ref*coord.y + norm2Ref*coord.y).Normalize();
    */
//    normalOut.y = norm0Ref.y*coord.y + norm1Ref.y*coord.y + norm2Ref.y*coord.y;
//    normalOut.z = norm0Ref.z*coord.z + norm1Ref.z*coord.z + norm2Ref.z*coord.z;

    Vector normalOuta = (vertex[1]-vertex[0]).Cross(vertex[2] - vertex[0]).Normalize();
    return normalOuta;
  }

  Vector GetReflectivity(Ray pRay, float intersectionTime)
  { // TODO: Base this off all vertcies
    SceneMaterialMgr &matMgr = SceneMaterialMgr::GetInstance();
    Vector out(0.0, 0.0, 0.0);
    Vector surfPt = pRay.GetPoint(intersectionTime);
    Vector coord = BarycentricCooef(surfPt);
    Vector mat0Ref = matMgr.GetMaterial(material[0]).reflective;
    Vector mat1Ref = matMgr.GetMaterial(material[1]).reflective;
    Vector mat2Ref = matMgr.GetMaterial(material[2]).reflective;

    out.x = mat0Ref.x*coord.x + mat1Ref.x*coord.y + mat2Ref.x*coord.y;
    out.y = mat0Ref.y*coord.y + mat1Ref.y*coord.y + mat2Ref.y*coord.y;
    out.z = mat0Ref.z*coord.z + mat1Ref.z*coord.z + mat2Ref.z*coord.z;
    return mat0Ref;
  }

  Vector GetRefractivity(Ray pRay, float intersectionTime)
  {
    SceneMaterialMgr &matMgr = SceneMaterialMgr::GetInstance();
    Vector out(0.0, 0.0, 0.0);
    Vector surfPt = pRay.GetPoint(intersectionTime);
    Vector coord = BarycentricCooef(surfPt);
    Vector mat0Ref = matMgr.GetMaterial(material[0]).refraction_index;
    Vector mat1Ref = matMgr.GetMaterial(material[1]).refraction_index;
    Vector mat2Ref = matMgr.GetMaterial(material[2]).refraction_index;

    out.x = mat0Ref.x*coord.x + mat1Ref.x*coord.y + mat2Ref.x*coord.y;
    out.y = mat0Ref.y*coord.y + mat1Ref.y*coord.y + mat2Ref.y*coord.y;
    out.z = mat0Ref.z*coord.z + mat1Ref.z*coord.z + mat2Ref.z*coord.z;
    return mat0Ref;
  }
};

/*
SceneModel Class - The model scene object

A model object consisting of a list of triangles derived from the SceneObject
*/
class SceneModel : public SceneObject
{
public:
  std::string filename;
  std::vector<SceneTriangle> triangleList;

//  int triangleIntersectionIdx;

  // -- Constructors & Destructors --
  SceneModel (void) : SceneObject ("Model", SceneObjectType::Model) {}
  SceneModel (std::string file) : SceneObject ("Model", SceneObjectType::Model) { filename = file; }
  SceneModel (std::string file, std::string nm) : SceneObject (nm, SceneObjectType::Model) { filename = file; }

  // -- Accessor Functions --
  // - GetNumTriangles - Returns the number of triangles in the model
  unsigned int GetNumTriangles (void) { return (unsigned int)triangleList.size (); }

  // - GetTriangle - Gets the nth SceneTriangle
  SceneTriangle *GetTriangle (int triIndex) { return &triangleList[triIndex]; }

//  void init(){triangleIntersectionIdx = -1;}
 
  float IntersectionTest(Ray pRay)
  {
    float t = FLT_MAX;
    for(int i = 0;i<triangleList.size();i++)
    {
      float potential_t = triangleList[i].IntersectionTest(pRay);
      Vector normal = triangleList[i].GetNormal(pRay,0.0);
      if(potential_t > 0 && potential_t<t)
      {
        t = potential_t;
//        triangleIntersectionIdx = i; // found intersecton so store
      }
    }
    if(t == FLT_MAX)
      t = -1.0;
    return t;
  }
  
  int intersectingTriangleIdx(Ray pRay)
  {
    float t = FLT_MAX;
    int triangleIntersectionIdx =-1;
    for(int i = 0;i<triangleList.size();i++)
    {
      float potential_t = triangleList[i].IntersectionTest(pRay);
      Vector normal = triangleList[i].GetNormal(pRay, 0.0);
      if(potential_t > 0 && potential_t<t)
      {
        t = potential_t;
        triangleIntersectionIdx = i; // found intersecton so store
      }
    }
    return triangleIntersectionIdx;
  }

  Vector GetColor(SceneLight pLight, Ray lookVector, float pDist)
  {
    Vector colorOut(0.0, 0.0, 0.0);
    int triangleIntersectionIdx =  intersectingTriangleIdx(lookVector);
    if(triangleIntersectionIdx >= 0.0 && triangleIntersectionIdx < triangleList.size())
    {
      colorOut = triangleList[triangleIntersectionIdx].GetColor(pLight, lookVector, pDist);
  //    triangleIntersectionIdx = -1;// reset since it was used
    }
    return colorOut;
  }
  
  Vector GetNormal(Ray pRay, float intersectionTime)
  {
    Vector normOut(0.0, 0.0, 0.0);
    int triangleIntersectionIdx =  intersectingTriangleIdx(pRay);
//    Vector surfPt = pRay.GetPoint(intersectionTime);
    if(triangleIntersectionIdx >= 0.0 && triangleIntersectionIdx < triangleList.size())
    {
      normOut = triangleList[triangleIntersectionIdx].GetNormal(pRay,intersectionTime);
  //    triangleIntersectionIdx = -1;// reset since it was used
    }
    return normOut;
  }

//  Vector GetReflectivity(Vector pSurfPt)
  Vector GetReflectivity(Ray pRay, float intersectionTime)
  {
    Vector refOut(0.0, 0.0, 0.0);
    int triangleIntersectionIdx =  intersectingTriangleIdx(pRay);
    if(triangleIntersectionIdx >= 0.0 && triangleIntersectionIdx < triangleList.size())
    {
      refOut = triangleList[triangleIntersectionIdx].GetReflectivity(pRay,intersectionTime);
    }
    return refOut;
  }

  Vector GetRefractivity(Ray pRay, float intersectionTime)
  {
    Vector refOut(0.0, 0.0, 0.0);
    int triangleIntersectionIdx =  intersectingTriangleIdx(pRay);
    if(triangleIntersectionIdx >= 0.0 && triangleIntersectionIdx < triangleList.size())
    {
      refOut = triangleList[triangleIntersectionIdx].GetRefractivity(pRay,intersectionTime);
    }
    return refOut;
  }


};

/*
Scene Class - The main scene definition class with the scene information

This is the base scene class
*/
class Scene
{
  std::string m_Desc, m_Author;
  SceneBackground m_Background;
  std::vector<SceneLight> m_LightList;
  std::vector<SceneObject *> m_ObjectList;

  // - Private utility Functions used by Load () -
  Vector ParseXYZ (XMLNode node)
  {
    if (node.isEmpty ())
      return Vector (0.0f, 0.0f, 0.0f);
    return Vector (atof(node.getAttribute("x")), 
      atof(node.getAttribute("y")),
      atof(node.getAttribute("z")));
  }

  void ParseOBJCommand (char *line, int max, char *command, int &position);
  Vector ParseOBJVector (char *str);
  bool ParseOBJCoords (char *str, int &num, int v_index[3], int n_index[3]);
public:
  Camera m_Camera;

  // -- Constructors & Destructors --
  Scene (void) 
  {}
  ~Scene (void)
  {
    // Free the memory allocated from the objects
    unsigned int numObj = GetNumObjects ();
    for (unsigned int n = 0; n < numObj; n++)
    {
      SceneObject *sceneObj = m_ObjectList[n];
      if (sceneObj->IsSphere ())
      {
        delete ((SceneSphere *)sceneObj);
      }
      else if (sceneObj->IsTriangle ())
      {
        delete ((SceneTriangle *)sceneObj);
      }
      else if (sceneObj->IsModel ())
      {
        delete ((SceneModel *)sceneObj);
      }
    }
    m_ObjectList.clear ();
  }

  // -- Main Functions --
  // - Load - Loads the Scene XML file
  bool Load (char *filename);

  // -- Accessor Functions --
  // - GetDescription - Returns the Description String
  const char * GetDescription (void) { return m_Desc.c_str(); }

  // - GetAuthor - Return the Author String
  const char * GetAuthor (void) { return m_Author.c_str(); }

  // - GetBackground - Returns the SceneBackground
  SceneBackground GetBackground (void) { return m_Background; }

  // - GetNumLights - Returns the number of lights in the scene
  unsigned int GetNumLights (void) { return (unsigned int)m_LightList.size (); }

  // - GetLight - Returns the nth SceneLight
  SceneLight GetLight (int lightIndex) { return m_LightList[lightIndex]; }

  // - GetNumMaterials - Returns the number of materials in the scene
  unsigned int GetNumMaterials (void)
  {
    SceneMaterialMgr& m_MaterialList = SceneMaterialMgr::GetInstance();
    return (unsigned int)m_MaterialList.size (); 
  }

  // - GetNumObjects - Returns the number of objects in the scene
  unsigned int GetNumObjects (void) { return (unsigned int)m_ObjectList.size (); }

  // - GetObject - Returns the nth object [NOTE: The Object will need to be type-casted afterwards]
  SceneObject* getObject (int objIndex) { return m_ObjectList[objIndex]; }

  // - GetCamera - Returns the camera class
  Camera GetCamera (void) { return m_Camera; }
};

bool Scene::Load (char *filename)
{
  XMLNode tempNode;

  // Open the Scene XML File
  printf ("Loading Scenefile %s...\n", filename);
  XMLNode sceneXML = XMLNode::openFileHelper(filename, "scene");
  if (sceneXML.isEmpty ())
    return false;
  m_Desc = CHECK_ATTR(sceneXML.getAttribute("desc"));
  m_Author = CHECK_ATTR(sceneXML.getAttribute("author"));

  printf ("Description: \n\t%s\n", m_Desc.c_str ());
  printf ("Author: %s\n", m_Author.c_str ());

  // Load the Background
  printf ("Loading Background...\n");
  tempNode = sceneXML.getChildNode("background");
  if (tempNode.isEmpty ())
    return false;
  m_Background.color = Tools::ParseColor (tempNode.getChildNode("color"));
  m_Background.ambientLight = Tools::ParseColor (tempNode.getChildNode("ambientLight"));

  // Load the Lights
  printf ("Loading Lights...\n");
  tempNode = sceneXML.getChildNode("light_list");
  if (!tempNode.isEmpty ())
  {
    unsigned int numLights = tempNode.nChildNode ("light");
    for (unsigned int n = 0; n < numLights; n++)
    {
      XMLNode tempLightNode = tempNode.getChildNode("light", n);
      if (tempLightNode.isEmpty ())
        return false;
      SceneLight tempLight;
      tempLight.color = Tools::ParseColor (tempLightNode.getChildNode("color"));
      tempLight.attenuationConstant = atof(CHECK_ATTR(tempLightNode.getChildNode("attenuation").getAttribute ("constant")));
      tempLight.attenuationLinear = atof(CHECK_ATTR(tempLightNode.getChildNode("attenuation").getAttribute ("linear")));
      tempLight.attenuationQuadratic = atof(CHECK_ATTR(tempLightNode.getChildNode("attenuation").getAttribute ("quadratic")));
      tempLight.ambiant = Tools::ParseColor (tempLightNode.getChildNode("ambiant"));
      tempLight.diffuse = Tools::ParseColor (tempLightNode.getChildNode("diffuse"));
      tempLight.specular = Tools::ParseColor (tempLightNode.getChildNode("specular"));
      tempLight.position = ParseXYZ (tempLightNode.getChildNode("position"));
      m_LightList.push_back (tempLight);
    }
  }

  // Load the Materials
  printf ("Loading Materials...\n");
  tempNode = sceneXML.getChildNode("material_list");
  if (!tempNode.isEmpty ())
  {
    SceneMaterialMgr& materialMgr = SceneMaterialMgr::GetInstance();
    if(!materialMgr.Load(tempNode))
      return false;
  }

  // Load the Objects
  printf ("Loading Objects...\n");
  tempNode = sceneXML.getChildNode("object_list");
  if (!tempNode.isEmpty ())
  {
    unsigned int numObjects = tempNode.nChildNode ();
    for (unsigned int n = 0; n < numObjects; n++)
    {
      XMLNode tempObjectNode = tempNode.getChildNode(n);
      if (tempObjectNode.isEmpty ())
        return false;
      if (!strcasecmp(tempObjectNode.getName (), "sphere"))
      {
        // Load a Sphere
        SceneSphere *tempSphere = new SceneSphere ();
        tempSphere->name = CHECK_ATTR(tempObjectNode.getAttribute("name"));
        tempSphere->material = CHECK_ATTR(tempObjectNode.getAttribute("material"));
        tempSphere->radius = atof(CHECK_ATTR(tempObjectNode.getAttribute("radius")));
        tempSphere->scale = ParseXYZ (tempObjectNode.getChildNode("scale"));
        tempSphere->rotation = ParseXYZ (tempObjectNode.getChildNode("rotation"));
        tempSphere->position = ParseXYZ (tempObjectNode.getChildNode("position"));
        tempSphere->center = ParseXYZ (tempObjectNode.getChildNode("center"));
        m_ObjectList.push_back (tempSphere);
      }
      else if (!strcasecmp(tempObjectNode.getName (), "triangle"))
      {
        // Load a Triangle
        XMLNode vertexNode;
        SceneTriangle *tempTriangle = new SceneTriangle ();
        tempTriangle->name = CHECK_ATTR(tempObjectNode.getAttribute("name"));
        tempTriangle->scale = ParseXYZ (tempObjectNode.getChildNode("scale"));
        tempTriangle->rotation = ParseXYZ (tempObjectNode.getChildNode("rotation"));
        tempTriangle->position = ParseXYZ (tempObjectNode.getChildNode("position"));

        // Load Vertex 0
        vertexNode = tempObjectNode.getChildNodeWithAttribute ("vertex", "index", "0");
        tempTriangle->material[0] = CHECK_ATTR(vertexNode.getAttribute("material"));
        tempTriangle->vertex[0] = ParseXYZ (vertexNode.getChildNode("position"));
        tempTriangle->normal[0] = ParseXYZ (vertexNode.getChildNode("normal"));
        tempTriangle->u[0] = atof (CHECK_ATTR(vertexNode.getChildNode("texture").getAttribute("u")));
        tempTriangle->v[0] = atof (CHECK_ATTR(vertexNode.getChildNode("texture").getAttribute("v")));

        // Load Vertex 1
        vertexNode = tempObjectNode.getChildNodeWithAttribute ("vertex", "index", "1");
        tempTriangle->material[1] = CHECK_ATTR(vertexNode.getAttribute("material"));
        tempTriangle->vertex[1] = ParseXYZ (vertexNode.getChildNode("position"));
        tempTriangle->normal[1] = ParseXYZ (vertexNode.getChildNode("normal"));
        tempTriangle->u[1] = atof (CHECK_ATTR(vertexNode.getChildNode("texture").getAttribute("u")));
        tempTriangle->v[1] = atof (CHECK_ATTR(vertexNode.getChildNode("texture").getAttribute("v")));

        // Load Vertex 2
        vertexNode = tempObjectNode.getChildNodeWithAttribute ("vertex", "index", "2");
        tempTriangle->material[2] = CHECK_ATTR(vertexNode.getAttribute("material"));
        tempTriangle->vertex[2] = ParseXYZ (vertexNode.getChildNode("position"));
        tempTriangle->normal[2] = ParseXYZ (vertexNode.getChildNode("normal"));
        tempTriangle->u[2] = atof (CHECK_ATTR(vertexNode.getChildNode("texture").getAttribute("u")));
        tempTriangle->v[2] = atof (CHECK_ATTR(vertexNode.getChildNode("texture").getAttribute("v")));

        m_ObjectList.push_back (tempTriangle);
      }
      else if (!strcasecmp(tempObjectNode.getName (), "model"))
      {
        // Load a Model
        SceneModel *tempModel = new SceneModel ();
        tempModel->filename = CHECK_ATTR(tempObjectNode.getAttribute("filename"));
        if (tempModel->filename == "")
        {
          printf ("No Specified Model filename\n");
          return false;
        }
        tempModel->name = CHECK_ATTR(tempObjectNode.getAttribute("name"));
        std::string material = CHECK_ATTR(tempObjectNode.getAttribute("material"));
        //tempModel->material = tempObjectNode.getAttribute("material");
        tempModel->scale = ParseXYZ (tempObjectNode.getChildNode("scale"));
        tempModel->rotation = ParseXYZ (tempObjectNode.getChildNode("rotation"));
        tempModel->position = ParseXYZ (tempObjectNode.getChildNode("position"));

        // Check the file format
        if (tempModel->filename.substr (tempModel->filename.length() - 4, 4) == ".3ds")
        {
          // Load the list of triangles from the .3ds
          C3DS sceneObj;
          if (!sceneObj.Create((char *)tempModel->filename.c_str()))
          {
            printf ("Error loading .3ds file\n");
            return false;
          }

          for (unsigned int obj = 0; obj < (unsigned int)sceneObj.m_iNumMeshs; obj++)
          {
            for (unsigned int n = 0; n < (unsigned int)sceneObj.m_pMeshs[obj].iNumFaces; n++)
            {
              SceneTriangle tempTriangle;

              Vector v1, v2, v3;
              // Vert 1
              v1.x = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[0]].x;
              v1.y = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[0]].y;
              v1.z = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[0]].z;
              // Vert 2
              v2.x = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[1]].x;
              v2.y = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[1]].y;
              v2.z = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[1]].z;
              // Vert 3
              v3.x = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[2]].x;
              v3.y = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[2]].y;
              v3.z = sceneObj.m_pMeshs[obj].pVerts[sceneObj.m_pMeshs[obj].pFaces[n].corner[2]].z;

              v1 = v1 + tempModel->position;
	      v2 = v2 + tempModel->position;
	      v3 = v3 + tempModel->position;

              // Normal
              Vector normal = (v1 - v2).Cross(v3 - v2).Normalize();

              // Load Vertex 0
              tempTriangle.material[0] = material;
              tempTriangle.vertex[0] = v1;
              tempTriangle.normal[0] = normal;
              // Texture Coords
              if (sceneObj.m_pMeshs[obj].bTextCoords)
              {
                tempTriangle.u[0] = sceneObj.m_pMeshs[obj].pTexs[sceneObj.m_pMeshs[obj].pFaces[n].corner[0]].tu;
                tempTriangle.v[0] = sceneObj.m_pMeshs[obj].pTexs[sceneObj.m_pMeshs[obj].pFaces[n].corner[0]].tv;
              }
              else
              {
                tempTriangle.u[0] = 0.0f;
                tempTriangle.v[0] = 0.0f;
              }

              // Load Vertex 1
              tempTriangle.material[1] = material;
              tempTriangle.vertex[1] = v2;
              tempTriangle.normal[1] = normal;
              // Texture Coords
              if (sceneObj.m_pMeshs[obj].bTextCoords)
              {
                tempTriangle.u[1] = sceneObj.m_pMeshs[obj].pTexs[sceneObj.m_pMeshs[obj].pFaces[n].corner[1]].tu;
                tempTriangle.v[1] = sceneObj.m_pMeshs[obj].pTexs[sceneObj.m_pMeshs[obj].pFaces[n].corner[1]].tv;
              }
              else
              {
                tempTriangle.u[1] = 0.0f;
                tempTriangle.v[1] = 0.0f;
              }

              // Load Vertex 2
              tempTriangle.material[2] = material;
              tempTriangle.vertex[2] = v3;
              tempTriangle.normal[2] = normal;
              // Texture Coords
              if (sceneObj.m_pMeshs[obj].bTextCoords)
              {
                tempTriangle.u[2] = sceneObj.m_pMeshs[obj].pTexs[sceneObj.m_pMeshs[obj].pFaces[n].corner[2]].tu;
                tempTriangle.v[2] = sceneObj.m_pMeshs[obj].pTexs[sceneObj.m_pMeshs[obj].pFaces[n].corner[2]].tv;
              }
              else
              {
                tempTriangle.u[2] = 0.0f;
                tempTriangle.v[2] = 0.0f;
              }

              tempModel->triangleList.push_back (tempTriangle);
            }
          }

          sceneObj.Release();
        }
        else if (tempModel->filename.substr (tempModel->filename.length() - 4, 4) == ".obj")
        {
          // The following code is a modified version of code from the old RayTracer Code rt_trimesh.cpp
          char line[MAX_LINE_LEN];
          char command[MAX_LINE_LEN];
          int position;
          vector<Vector> vertices;
          vector<Vector> normals;

          std::ifstream infile (tempModel->filename.c_str());

          if (infile.fail())
          {
            printf ("Error loading .obj file\n");
            return false;
          }

          while (infile.good ())
          {
            infile.getline (line, MAX_LINE_LEN);
            ParseOBJCommand (line, MAX_LINE_LEN, command, position);

            if (strcmp (command,"v")==0)
            {
              Vector pos = ParseOBJVector (&(line[position]));
              vertices.push_back (pos);
            }
            else if (strcmp (command,"vn")==0)
            {
              Vector norm = ParseOBJVector (&(line[position]));
              normals.push_back (norm);
            }
            else if (strcmp (command,"f")==0)
            {
              int num = 0; // number of edges
              int v_index[3]; // vertex index
              int n_index[3]; // normal index

              if (!ParseOBJCoords (&(line[position]), num, v_index, n_index))
              {
                printf ("Error parsing faces in .obj file\n");
                return false;
              }

              SceneTriangle tempTriangle;

              // Load Vertex 0
              tempTriangle.material[0] = material;
              tempTriangle.vertex[0] = vertices[v_index[0]];
	      tempTriangle.vertex[0] = tempTriangle.vertex[0] + tempModel->position;
              tempTriangle.normal[0] = normals[n_index[0]];
              // Texture Coords
              tempTriangle.u[0] = 0.0f;
              tempTriangle.v[0] = 0.0f;

              // Load Vertex 1
              tempTriangle.material[1] = material;
              tempTriangle.vertex[1] = vertices[v_index[1]];
	      tempTriangle.vertex[1] = tempTriangle.vertex[1] + tempModel->position;
              tempTriangle.normal[1] = normals[n_index[1]];
              // Texture Coords
              tempTriangle.u[1] = 0.0f;
              tempTriangle.v[1] = 0.0f;

              // Load Vertex 2
              tempTriangle.material[2] = material;
              tempTriangle.vertex[2] = vertices[v_index[2]];
	      tempTriangle.vertex[2] = tempTriangle.vertex[2] + tempModel->position;
              tempTriangle.normal[2] = normals[n_index[2]];
              // Texture Coords
              tempTriangle.u[2] = 0.0f;
              tempTriangle.v[2] = 0.0f;

              tempModel->triangleList.push_back (tempTriangle);
            }
            else
            {
              //printf ("Ignoring command <%s> in obj file\n", command);
            }
          }
          infile.close ();
        }
        else
        {
          printf ("Unsupported file format\n");
          return false;
        }

        m_ObjectList.push_back (tempModel);
      }
      else
      {
        printf ("Unrecognized Node <%s> in <object_list>\n", tempObjectNode.getName ());
        exit (255);
      }
    }
  }

  // Load the Camera
  printf ("Loading Camera...\n");
  tempNode = sceneXML.getChildNode("camera");
  if (tempNode.isEmpty ())
    return false;
  m_Camera.SetFOV (atof(CHECK_ATTR2(tempNode.getAttribute("fieldOfView"), "45.0")));
  m_Camera.SetNearClip (atof(CHECK_ATTR2(tempNode.getAttribute("nearClip"), "0.1")));
  m_Camera.SetFarClip (atof(CHECK_ATTR2(tempNode.getAttribute("farClip"), "100.0")));
  m_Camera.SetPosition (ParseXYZ (tempNode.getChildNode("position")));
  m_Camera.SetTarget (ParseXYZ (tempNode.getChildNode("target")));
  m_Camera.SetUp (ParseXYZ (tempNode.getChildNode("up")));

  printf ("Scene Loaded!\n");

  return true;
}

void Scene::ParseOBJCommand (char *line, int max, char *command, int &position)
{
  int i = 0;
  int j = 0;

  while (i<max && line[i]==' ') i++;
  while (i<max && line[i]!='\0' && line[i]!=' ')
  {
    command[j] = line[i];
    j++; i++;
  }

  command[j] = '\0';
  position = i;
}

Vector Scene::ParseOBJVector (char *str)
{
  int i = 0;
  float x,y,z;
  while (str[i]!='\0' && str[i]==' ') i++;
  x = atof (&(str[i]));
  while (str[i]!='\0' && str[i]!=' ') i++;
  y = atof (&(str[i]));
  while (str[i]!='\0' && str[i]==' ') i++;
  while (str[i]!='\0' && str[i]!=' ') i++;
  z = atof (&(str[i]));
  return Vector (x,y,z);
}

bool Scene::ParseOBJCoords (char *str, int &num, int v_index[3], int n_index[3])
{
  int i = 0;
  num = 0;

  while (str[i]!='\0' && str[i]==' ') i++;
  while (str[i]!='\0')
  {
    while (str[i]!='\0' && str[i]!=' ') i++;
    while (str[i]!='\0' && str[i]==' ') i++;
    num++;
  }
  if (num != 3)
    return false;

  i = 0;
  for (int j=0; j<num; j++)
  {
    while (str[i]==' ') i++;
    v_index[j] = atoi (&(str[i])) - 1;
    while (str[i]!='/' && str[i]!=' ') i++;
    if (str[i] != '/')
      return false;
    i++;
    while (str[i]!='/' && str[i]!=' ') i++;
    if (str[i] != '/')
      return false;
    i++;
    n_index[j] = atoi (&(str[i])) - 1;
    while(str[i]!='\0' && str[i]!=' ') i++;
  }

  return true;
}

#endif // SCENE_H
