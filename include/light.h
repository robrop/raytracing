#ifndef LIGHT_H
#define LIGHT_H

#include "vector3.h"

class Light {
public:
  Vector3 position;
  Vector3 intensity;

  Light() : position(Vector3()), intensity(Vector3()) {}
  Light(Vector3 _intensity) : position(Vector3()), intensity(_intensity) {}
  Light(Vector3 _position, Vector3 _intensity)
      : position(_position), intensity(_intensity) {}
};

#endif
