#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"

class Sphere : public Shape {
public:
  float radius, radius2;

  Sphere(const Vector3 &_center, const float _radius, const Color &_color,
         const float _ka, const float _kd, const float _ks)
      : radius(_radius), radius2(_radius * _radius) {
    center = _center;
    color = _color;
    ka = _ka;
    kd = _kd;
    ks = _ks;
  }

  // Compute a ray-sphere intersection using the geometric method
  bool intersect(const Ray &ray, float &t0, float &t1) {
    Vector3 l = center - ray.origin;
    float tca = l.dot(ray.direction); // Closest approach
    if (tca < 0)
      return false; // Ray intersection behind ray origin
    float d2 = l.dot(l) - tca * tca;
    if (d2 > radius2)
      return false;                 // Ray doesn't intersect
    float thc = sqrt(radius2 - d2); // Closest approach to surface of sphere
    t0 = tca - thc;
    t1 = tca + thc;
    return true;
  }

  Vector3 getNormal(const Vector3 &hitPoint) {
    return (hitPoint - center) / radius;
  }
};

#endif
