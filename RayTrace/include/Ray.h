#ifndef __RAY_H__
#define __RAY_H__

#include <string>
#include <sstream>
#include "Utils.h"

class Ray
{
    public:
        Ray(Vector pOriginPos, Vector pDirVec);

        Ray(Vector pOriginPos, Vector pDirVec, Vector endpoint);

        std::string ToString();

        Vector GetPoint(const float t) {return mStart + mDirection * t;}

        Vector GetOrigin(){return mStart;}
        Vector GetDirection(){return mDirection;}

    private:
        Vector mStart;
        Vector mDirection;
        Vector mEndpoint;
};

#endif
