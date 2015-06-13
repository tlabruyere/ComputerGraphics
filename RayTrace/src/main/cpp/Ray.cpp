#include "Ray.h"

Ray::Ray(Vector pOriginPos, Vector pDirVec)
{
    mStart = pOriginPos;
    mDirection = pDirVec.Normalize();
}

Ray::Ray(Vector pOriginPos, Vector pDirVec, Vector endpoint)
{
    mStart = pOriginPos;
    mDirection = pDirVec.Normalize();
    mEndpoint = endpoint;
}

std::string Ray::ToString()
{
    std::stringstream str;
    str<< "Point: <" << mStart.x << "," << mStart.y << "," << mStart.z << "> ";
    str<< "Direction <" <<mDirection.x << "," <<mDirection.y << ","  <<mDirection.z << "> " ;
    //    str<< "Endpoint <" <<mEndpoint.x << "," <<mEndpoint.y << ","  <<mEndpoint.z << ">" ;
    return str.str();
}


