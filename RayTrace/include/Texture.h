#ifndef TEXTURE_H
#define TEXTURE_H

#include "Utils.h"
#include <cmath>

class Texture
{
public:
  static Vector CheckerBoardTexture(float u, float v)
  {
    Vector colorOut(0.0, 0.0, 0.0); // set to black
    float checkerSize = 40.0;
    int modSize = 4;
    int halfSize = 2;
    int scaleU =(u *checkerSize) ;
    int scaleV =(v *checkerSize) ;

    float modU = scaleU % modSize;
    float modV = scaleV % modSize;

    if( modU < halfSize && (modV < halfSize) )
    {
      colorOut = Vector(1.0,0.0,0.0); // set to white
    }
    if( modU >= halfSize && (modV >= halfSize) )
    {
      colorOut = Vector(1.0,0.0,0.0); // set to white
    }
    if( modU < halfSize && (modV >= halfSize) )
    {
      colorOut = Vector(0.0,1.0,1.0); // set to white
    }
    if( modU >= halfSize && (modV < halfSize) )
    {
      colorOut = Vector(0.0,1.0,1.0); // set to white
    }


    return colorOut;

  }
};
#endif