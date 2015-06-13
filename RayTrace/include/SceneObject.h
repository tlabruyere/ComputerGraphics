/**
 * Author: Tom LaBruyere
 * file: SceneObject.h
 */
#ifndef SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEOBJECT_H__
#define SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEOBJECT_H__
#include "SceneObjectType.h"
/**
 * SceneObject Class - A base object class that defines the common features of 
 *  all objects. This is the base object class that the various scene object 
 *  types derive from
 */
class SceneObject
{
    public:
        std::string name;
        SceneObjectType::ObjectType type;
        Vector scale, rotation, position;

        // -- Constructors & Destructors --
        SceneObject() : 
            scale( 1.0f, 1.0f, 1.0f)
        {}

        SceneObject(SceneObjectType::ObjectType tp) : 
            type (tp),
            scale( 1.0f, 1.0f, 1.0f)
        {}

        SceneObject(std::string nm, SceneObjectType::ObjectType tp) : 
            name(nm), 
            type(tp),
            scale( 1.0f, 1.0f, 1.0f)
        {}

        // -- Object Type Checking Functions --
        bool IsSphere() const 
        { 
            return (type == SceneObjectType::Sphere); 
        }
        bool IsTriangle() const 
        { 
            return (type == SceneObjectType::Triangle); 
        }
        bool IsModel() const 
        { 
            return (type == SceneObjectType::Model); 
        }

        /* 
         * Tests for intersection between Ray and object
         * If intersection returns the t value in the parametric equation 
         *    P(x) = Po + t*Pd so the point intersection can be computed
         * else return a negitive number classifying there is no intersection 
         * in the direction of the ray
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

#endif // END SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEOBJECT_H__
