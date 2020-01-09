#ifndef SHAPE_H
#define SHAPE_H

#include "color.h"
#include "ray.h"

class Shape {
public:
  Vector3 center;   // Position
  Color color;      // Surface Diffuse Color
  float ka, kd, ks; // Ambient, Diffuse, Specular Coefficents

  float kt = 0;

  virtual bool intersect(const Ray &ray, float &t0, float &t1) { return false; }
  virtual Vector3 getNormal(const Vector3 &hitPoint) { return Vector3(); }
};

#endif
