// Fall 2019

#pragma once

#include <iosfwd>
#include <glm/glm.hpp>
#include "Primitive.hpp"

using namespace glm;

// Represents a simple point light.
class Light {
public:
// ctors
  Light();
  Light(vec3 col, vec3 pos);

  vec3 colour;
  vec3 position;
  double falloff[3];
  // type indicator
  virtual dvec3 randomPoint();
  virtual bool usingALight();
};

class AreaLight : public Light {
public:
  // ctor
  AreaLight(vec3 col, vec3 pos);
  Primitive prim;
  // type indicator
  virtual dvec3 randomPoint() = 0;
  virtual bool usingALight();
};

struct SphericalLight : AreaLight {
  SphericalLight(vec3 col, vec3 pos, double radius);
  double sphere_r;
  virtual dvec3 randomPoint();
};

std::ostream& operator<<(std::ostream& out, const Light& l);
