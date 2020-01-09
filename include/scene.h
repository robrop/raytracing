#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "light.h"
#include "ray.h"
#include "shape.h"
#include <vector>

class Scene {
public:
  std::vector<Shape *> objects;
  std::vector<Light *> lights;
  Light ambientLight;
  Color backgroundColor;
  Camera camera;

  Scene(Camera _camera) : camera(_camera) { backgroundColor = Color(); }
  void addAmbientLight(Light _light) { ambientLight = _light; }
  void addLight(Light *_light) { lights.push_back(_light); }
  void addObject(Shape *_object) { objects.push_back(_object); }

  Shape *intersect(const Ray &ray, float &tnear) const {
    tnear = INFINITY;
    Shape *hit = NULL;
    // Find nearest intersection with ray and objects in scene
    for (int i = 0; i < objects.size(); i++) {
      float t0 = INFINITY, t1 = INFINITY;
      if (objects[i]->intersect(ray, t0, t1)) {
        if (t0 < 0)
          t0 = t1;
        if (t0 < tnear) {
          tnear = t0;
          hit = objects[i];
        }
      }
    }
    return hit;
  }

  Shape *intersectFist(const Ray &ray, float &tnear) const {
    tnear = INFINITY;
    // Find nearest intersection with ray and objects in scene
    for (int i = 0; i < objects.size(); i++) {
      float t0 = INFINITY, t1 = INFINITY;
      if (objects[i]->intersect(ray, t0, t1)) {
        if (t0 < 0)
          t0 = t1;
        if (t0 < tnear) {
          tnear = t0;
          return objects[i];
        }
      }
    }
    return NULL;
  }

  bool inShadow(const Vector3 &point, const Light &light) const {
    Vector3 shadowRayDirection = light.position - point;
    shadowRayDirection.normalize();
    Ray shadowRay(point, shadowRayDirection);
    float tnear;
    return intersectFist(shadowRay, tnear) != NULL;
  }
};

#endif
