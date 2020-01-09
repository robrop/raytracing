#ifndef RAY_H
#define RAY_H

#include "vector3.h"

class Ray {
public:
  Vector3 origin;
  Vector3 direction;
  Ray(Vector3 _origin, Vector3 _direction)
      : origin(_origin), direction(_direction) {}
};

#endif
