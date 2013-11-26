#pragma once
#include <string>
#include <sstream>
#include "Utils.h"

class Ray
{
private:
  Vector _start;
  Vector _direction;
  Vector _endpoint;

public:
  Ray(Vector pOriginPos, Vector pDirVec)
  {
    _start = pOriginPos;
    _direction = pDirVec.Normalize();
  }

  Ray(Vector pOriginPos, Vector pDirVec, Vector endpoint)
  {
    _start = pOriginPos;
    _direction = pDirVec.Normalize();
    _endpoint = endpoint;
  }

  std::string ToString()
  {
    std::stringstream str;
    str<< "Point: <" << _start.x << "," << _start.y << "," << _start.z << "> ";
    str<< "Direction <" <<_direction.x << "," <<_direction.y << ","  <<_direction.z << "> " ;
//    str<< "Endpoint <" <<_endpoint.x << "," <<_endpoint.y << ","  <<_endpoint.z << ">" ;
    return str.str();
  }

  Vector GetOrigin(){return _start;}
  Vector GetDirection(){return _direction;}

};

