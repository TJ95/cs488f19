// Fall 2019

#include <iostream>
#include <glm/ext.hpp>
#include "Light.hpp"

using namespace glm;
/* -----------------------
      Point Light
--------------------------*/
Light::Light() : colour(0.0, 0.0, 0.0), position(0.0, 0.0, 0.0) {
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

Light::Light(vec3 col, vec3 pos) : colour(col), position(pos) {
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

dvec3 Light::randomPoint() {
  return position;
}

bool Light::usingALight() {
  return false;
}

// overloading "<<" for Light
std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << to_string(l.colour) 
  	  << ", " << to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}

/* -----------------------
      Area Light
--------------------------*/
AreaLight::AreaLight(vec3 col, vec3 pos) : Light(col, pos) {}

bool AreaLight::usingALight(){
  return true;
}

/* -----------------------
      Spherical Light
--------------------------*/
SphericalLight::SphericalLight(vec3 col, vec3 pos, double radius) : AreaLight(col, pos), sphere_r(radius) {
  prim = NonhierSphere(pos, sphere_r);
}

dvec3 SphericalLight::randomPoint() {
  dvec3 vecRay = ballRand(sphere_r);
  dvec3 point = dvec3(position) + vecRay;
  return point;
}