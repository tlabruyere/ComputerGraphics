/**
 * Author: Tom LaBruyere
 * file: SceneTriangle.h
 */
#ifndef SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENETRIANGLE_H__ 
#define SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENETRIANGLE_H__ 
/**
 * SceneTriangle Class - The triangle scene object
 * Single triangle object derived from the SceneObject
 */
class SceneTriangle : public SceneObject
{
    public:
        std::string material[3];
        Vector vertex[3];
        Vector normal[3];
        float u[3], v[3];

        // -- Constructors & Destructors --
        SceneTriangle (void) : 
            SceneObject ("Triangle", SceneObjectType::Triangle) 
        {}

        SceneTriangle (std::string nm) : 
            SceneObject (nm, SceneObjectType::Triangle) 
        {}

        float IntersectionTest(Ray pRay);

        Vector GetColor(SceneLight pLight, Ray lookVector, float pDist);

        Vector GetNormal(Ray pRay, float intersectionTime);

        Vector GetReflectivity(Ray pRay, float intersectionTime);

        Vector GetRefractivity(Ray pRay, float intersectionTime);

    private:
        Vector BarycentricCooef(Vector point);
};

#endif // END SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENETRIANGLE_H__ 
