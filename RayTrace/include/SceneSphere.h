/**
 * Author: Tom LaBruyere
 * file: SceneSphere.h
 */
#ifndef SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENESPHERE_H__ 
#define SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENESPHERE_H__ 
/**
 * SceneSphere Class - The sphere scene object
 * Sphere object derived from the SceneObject
 */
class SceneSphere : public SceneObject
{
    public:
        std::string material;
        Vector center;
        float radius;

        // -- Constructors & Destructors --
        SceneSphere() : 
            SceneObject("Sphere", SceneObjectType::Sphere) 
        {}

        SceneSphere(std::string nm) : 
            SceneObject (nm, SceneObjectType::Sphere) 
        {}

        float IntersectionTest(Ray pRay);

        Vector GetColor(SceneLight pLight, Ray lookVector, float pDist);

        Vector GetNormal(Ray pRay, float intersectionTime);

        Vector GetReflectivity(Ray pRay, float intersectionTime);

        Vector GetRefractivity(Ray pRay, float intersectionTime);
};


#endif // END SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENESPHERE_H__ 
