/**
 * NOTE: You do not need to edit this file for this assignment but may do so
 * 
 * This file defines the following:
 * SceneObjectType Enumeration
 * SceneBackground Class
 * SceneLight Class
 * SceneMaterial Class
 * SceneObject Class -> SceneSphere, SceneTriangle, SceneModel
 * Scene Class
 * 
 * Scene Usage: Several Functions
 * Scene::Load(sceneFile) - load a scene descriptor file
 * 
 * Scene::GetDescription() - get the scene description string
 * Scene::GetAuthor() - get the scene author string
 * Scene::GetBackground() - get the scene background information
 * Scene::GetNumLights() - get the number of lights in the scene
 * Scene::GetLight(lightIndex) - get one of the lights in the scene
 * Scene::GetNumMaterials() - get the number of materials in the scene
 * Scene::GetMaterial(materialIndex OR materialName) - get a material's description
 * Scene::GetNumObjects() - get the number of objects in the scene
 * Scene::GetObject(objectIndex) - get an object's description
 * Scene::GetCamera() - get the current camera description
 * 
 * The trickiest one of these is the GetObject () function
 * It is used like this:
 * [The object is type-casted to its corresponding type]
 * 
 * SceneObject tempObject = m_Scene.GetObject (0);
 * if (tempObject.IsTriangle ())
 * {
 *   Vector vertices[3];
 * 
 *   for (int n = 0; n < 3; n++)
 *   {
 *    vertices[n] = ((SceneTriangle)tempObject).vertex[n];
 *   }
 * }
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
#include "SceneBackground.h"
#include "SceneObject.h"
#include "SceneObjectType.h"
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

/**
 * Scene Class - The main scene definition class with the scene information
 * This is the base scene class
 */
class Scene
{
    public:
        Camera m_Camera;

        // -- Constructors & Destructors --
        Scene() 
        {}

        ~Scene(void);

        // -- Main Functions --
        // - Load - Loads the Scene XML file
        bool Load(char *filename);

        // -- Accessor Functions --
        // - GetDescription - Returns the Description String
        const char* GetDescription(void) { return m_Desc.c_str(); }

        // - GetAuthor - Return the Author String
        const char* GetAuthor(void) { return m_Author.c_str(); }

        // - GetBackground - Returns the SceneBackground
        SceneBackground GetBackground(void) { return m_Background; }

        // - GetNumLights - Returns the number of lights in the scene
        unsigned int GetNumLights(void) { return (unsigned int)m_LightList.size (); }

        // - GetLight - Returns the nth SceneLight
        SceneLight GetLight(int lightIndex) { return m_LightList[lightIndex]; }

        // - GetNumMaterials - Returns the number of materials in the scene
        unsigned int GetNumMaterials(void);

        // - GetNumObjects - Returns the number of objects in the scene
        unsigned int GetNumObjects (void) { return (unsigned int)m_ObjectList.size (); }

        // - GetObject - Returns the nth object [NOTE: The Object will need to be type-casted afterwards]
        SceneObject* getObject (int objIndex) { return m_ObjectList[objIndex]; }

        // - GetCamera - Returns the camera class
        Camera GetCamera (void) { return m_Camera; }
    
    private:
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

};
#endif // SCENE_H
