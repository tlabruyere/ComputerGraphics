/**
 * Author: Tom LaBruyere
 * file: SceneModel.h
 */
#ifndef SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEMODEL_H__ 
#define SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEMODEL_H__ 
/**
 * SceneModel Class - The model scene object
 * A model object consisting of a list of triangles derived from the SceneObject
 */
class SceneModel : public SceneObject
{
    public:
        std::string filename;
        std::vector<SceneTriangle> triangleList;

        //  int triangleIntersectionIdx;

        // -- Constructors & Destructors --
        SceneModel(void) : 
            SceneObject ("Model", SceneObjectType::Model) 
        {}

        SceneModel(std::string file) : 
            SceneObject ("Model", SceneObjectType::Model),
            filename(file)
        {}

        SceneModel(std::string file, std::string nm) : 
            SceneObject (nm, SceneObjectType::Model),
            filename(file)
        {}

        // -- Accessor Functions --
        // - GetNumTriangles - Returns the number of triangles in the model
        unsigned int GetNumTriangles(void) { return (unsigned int)triangleList.size (); }

        // - GetTriangle - Gets the nth SceneTriangle
        SceneTriangle *GetTriangle(int triIndex) { return &triangleList[triIndex]; }

        //  void init(){triangleIntersectionIdx = -1;}

        float IntersectionTest(Ray pRay);

        int intersectingTriangleIdx(Ray pRay);

        Vector GetColor(SceneLight pLight, Ray lookVector, float pDist);

        Vector GetNormal(Ray pRay, float intersectionTime);

        //  Vector GetReflectivity(Vector pSurfPt)
        Vector GetReflectivity(Ray pRay, float intersectionTime);

        Vector GetRefractivity(Ray pRay, float intersectionTime);
};

#endif // END SCHOOL_COMPUTERGRAPHICS_RAYTRACE_SCENEMODEL_H__ 
