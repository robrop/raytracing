struct Color;
class Light;
class Shape;
class Scene;
class Sphere;

#define MAX_RAY_DEPTH 8

#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include <fstream>
#include <iostream>
#include <thread>

using namespace std;

Color trace(const Scene &scene, const Ray &ray, const int &depth);
void drawImage(Color *image, int width, int height, const char *name);

Color getLighting(const Shape &object, const Vector3 &point,
                  const Vector3 &normal, const Vector3 &view,
                  const Light *light) {
  Color rayColor;

  // Create diffuse color
  Vector3 N = normal;

  Vector3 L = light->position - point;
  float distance = L.length();
  L.normalize();

  float NdotL = N.dot(L);
  float intensity = max(0.0f, NdotL);
  Color diffuse = object.color * light->intensity * intensity;

  rayColor = diffuse * object.kd;

  return rayColor;
}

Color trace(const Scene &scene, const Ray &ray, const int &depth) {
  Color rayColor;
  float tnear;
  Shape *hit = scene.intersect(ray, tnear);

  if (!hit) {
    if (depth < 1)
      return scene.backgroundColor;
    else
      return Color();
  }
  Vector3 hitPoint = ray.origin + ray.direction * tnear;
  Vector3 N = hit->getNormal(hitPoint);
  N.normalize();
  Vector3 V = scene.camera.position - hitPoint;
  V.normalize();

  rayColor = hit->color * scene.ambientLight.intensity * hit->ka;

  for (int i = 0; i < scene.lights.size(); i++) {
    if (!scene.inShadow(hitPoint, *scene.lights[i]))
      rayColor += getLighting(*hit, hitPoint, N, V, scene.lights[i]);
  }

  float bias = 1e-4;
  bool inside = false;
  if (ray.direction.dot(N) > 0)
    N = -N, inside = true;
  if (hit->ks > 0 && depth < MAX_RAY_DEPTH) {
    // Compute Reflection Ray and Color
    Vector3 R = ray.direction - N * 2 * ray.direction.dot(N);
    R.normalize();

    Ray rRay(hitPoint + N * bias, R);
    float VdotR = max(0.0f, V.dot(-R));
    Color reflectionColor = trace(scene, rRay, depth + 1); //* VdotR;
    rayColor += reflectionColor * hit->ks;
  }

  if (hit->kt > 0 && depth < MAX_RAY_DEPTH) {
    // Compute Refracted Ray (transmission ray) and Color
    float ni = 1.1;
    float nt = 1.0;
    float nit = ni / nt;
    if (inside)
      nit = 1 / nit;
    float costheta = -N.dot(ray.direction);
    float k = 1 - nit * nit * (1 - costheta * costheta);
    Vector3 T = ray.direction * nit + N * (nit * costheta - sqrt(k));
    T.normalize();

    Ray refractionRay(hitPoint - N * bias, T);
    Color refractionColor = trace(scene, refractionRay, depth + 1);
    rayColor += refractionColor * hit->kt;
  }
  return rayColor;
}

void drawImage(Color *image, int width, int height, const char *name) {
  ofstream out(name, std::ios::out | std::ios::binary);
  out << "P6\n" << width << " " << height << "\n255\n";
  for (unsigned i = 0; i < width * height; i++) {
    Color pixel = image[i].clamp();
    out << (unsigned char)(pixel.r * 255);
    out << (unsigned char)(pixel.g * 255);
    out << (unsigned char)(pixel.b * 255);
  }
  out.close();
}

void loop(vector<Color> &image, Camera const &camera, Scene const &scene,
          clock_t t, int samples, int width, int height, int start, int end,
          bool output = false) {
  vector<Color> temp_image(width * height);
  for (int y = start; y < end; y++) {
    // if(output)
    // {
    //     float elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
    //     float perc = (float)y / (float)height;
    //     printf ("%12.6fs: %5d/%d: %3d%%, estimate: %12.6fs\n", elaps, y,
    //     height,(int)(perc*100), elaps/perc); fflush(stdout);
    // }
    for (int x = 0; x < width; x++) {
      int pixel = y * width + x;
      for (int s = 0; s < samples; s++) {
        Vector3 r = Vector3::random();
        float jx = x + r.x;
        float jy = y + r.y;
        if (s == 0) {
          jx = x;
          jy = y;
        }

        // Send a jittered ray through each pixel
        Vector3 rayDirection = camera.pixelToViewport(Vector3(jx, jy, 1));

        Ray ray(camera.position, rayDirection);

        // Sent pixel for traced ray
        temp_image[pixel] += trace(scene, ray, 0);
      }
      temp_image[pixel] = temp_image[pixel] / samples;
    }
  }
  for (int y = start; y < end; y++) {
    for (int x = 0; x < width; x++) {
      int pixel = y * width + x;
      image[pixel] = temp_image[pixel];
    }
  }
}

int main(int argc, const char *argv[]) {
  const char *name;
  int width = 1920, height = 1080, samples = 16;
  if (argc >= 2) {
    if (argc >= 3) {
      if (argc >= 5) {
        name = argv[1];
        samples = atoi(argv[2]);
        width = atoi(argv[3]);
        height = atoi(argv[4]);
      } else {
        name = argv[1];
        samples = atoi(argv[2]);
      }
    } else {
      name = argv[1];
    }
  } else {
    name = "./scene.ppm";
  }

  clock_t t, t_run;
  float elaps, elaps2;
  t = clock();
  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Generating Scene ...\n", elaps);

  vector<Color> image(width * height);
  fill(image.begin(), image.end(), 0);
  Camera camera = Camera(Vector3(0, 0, -20), width, height, 30);
  camera.position = Vector3(0, 20, -20);
  camera.angleX = 30 * M_PI / 180.0;

  Scene scene = Scene(camera);
  scene.backgroundColor = Color();
  scene.addAmbientLight(Light(Vector3(0.5)));
  Light l0 = Light(Vector3(0, 20, 35), Vector3(1.4));
  scene.addLight(&l0);
  Light l1 = Light(Vector3(20, 20, 35), Vector3(1.8));
  scene.addLight(&l1);

  Sphere s0 = Sphere(Vector3(0, -10004, 20), 10000, Color(0.2, 0.2, 0.2), 0.2,
                     0.5, 0.0);
  scene.addObject(&s0);
  Sphere s1 = Sphere(Vector3(0, 0, 20), 4, Color(1, 0, 0), 0.1, 0.1, 0.0);
  s1.kt = 0.8;
  scene.addObject(&s1);
  Sphere s2 = Sphere(Vector3(5, -1, 15), 2, Color(1, 0.7, 0.1), 0.4, 0.6,
                     0.4); // Yellow
  scene.addObject(&s2);
  Sphere s3 =
      Sphere(Vector3(5, 0, 25), 3, Color(0, 0.15, 0.3), 0.3, 0.8, 0.1); // Blue
  scene.addObject(&s3);
  Sphere s4 = Sphere(Vector3(-3.5, -1, 10), 2, Color(0, 0.2, 0.15), 0.4, 0.6,
                     0.5); // Green
  scene.addObject(&s4);
  Sphere s5 = Sphere(Vector3(-5.5, 0, 15), 3, Color(0.1, 0.1, 0.1), 0.3, 0.8,
                     0.25); // Black
  scene.addObject(&s5);
  Sphere s6 =
      Sphere(Vector3(0, 0, 25), 1, Color(0.6, 0.6, 0.6), 0.4, 1, 0); // Green
  scene.addObject(&s6);

  int num_threads = thread::hardware_concurrency();

  int per_thread = height / num_threads;
  int remaining = height % num_threads;

  thread threads[num_threads];

  printf("per: %d rem: %d\n", per_thread, remaining);

  // auto f = [&image, &camera, &scene, t, samples, width, height](int start,
  // int end, bool output=false) {
  //     for (int y = start; y < end; y++) {
  //         // if(output)
  //         // {
  //         //     float elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  //         //     float perc = (float)y / (float)height;
  //         //     printf ("%12.6fs: %5d/%d: %3d%%, estimate: %12.6fs\n",
  //         elaps, y, height,(int)(perc*100), elaps/perc);
  //         //     fflush(stdout);
  //         // }
  //         for (int x = 0; x < width; x++) {
  //             int pixel = y * width + x;
  //             for (int s = 0; s < samples; s++) {
  //                 Vector3 r = Vector3::random();
  //                 float jx = x + r.x;
  //                 float jy = y + r.y;
  //                 if(s == 0){
  //                     jx = x;
  //                     jy = y;
  //                 }
  //
  //                 // Send a jittered ray through each pixel
  //                 Vector3 rayDirection = camera.pixelToViewport( Vector3(jx,
  //                 jy, 1) );
  //
  //                 Ray ray(camera.position, rayDirection);
  //
  //                 // Sent pixel for traced ray
  //                 image[pixel] += trace(scene, ray, 0);
  //             }
  //             image[pixel] = image[pixel] / samples;
  //         }
  //     }
  // };

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Renderering Scene ...\n", elaps);

  t_run = clock();

  loop(ref(image), ref(camera), ref(scene), t, samples, width, height, 0,
       height, true);

  elaps2 = ((float)clock() - t_run) / CLOCKS_PER_SEC;

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("\r%12.6fs: %5d/%d: 100%%\n", elaps, height, height);
  printf("%12.6fs\n", elaps2);

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Writing Scene ...\n", elaps);

  drawImage(image.data(), width, height, "test2.ppm");

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Scene Complete.\n", elaps);

  fill(image.begin(), image.end(), 0);

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Renderering Scene ...\n", elaps);

  t_run = clock();

  for (int i = 0; i < num_threads; i++) {
    threads[i] =
        std::thread(loop, ref(image), ref(camera), ref(scene), t, samples,
                    width, height, i * per_thread, (i + 1) * per_thread, true);
  }

  if (remaining > 0)
    loop(ref(image), ref(camera), ref(scene), t, samples, width, height,
         height - remaining, height, true);

  for (int i = 0; i < num_threads; i++) {
    threads[i].join();
  }

  elaps2 = ((float)clock() - t_run) / CLOCKS_PER_SEC;

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("\r%12.6fs: %5d/%d: 100%%\n", elaps, height, height);
  printf("%12.6fs\n", elaps2);

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Writing Scene ...\n", elaps);

  drawImage(image.data(), width, height, "test.ppm");

  elaps = ((float)clock() - t) / CLOCKS_PER_SEC;
  printf("%12.6fs: Scene Complete.\n", elaps);
}
