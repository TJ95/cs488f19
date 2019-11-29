#pragma once

#include <stdio.h>
#include <glm/ext.hpp>
#include "PhongMaterial.hpp"
using namespace glm;

class ray {
public:
    dvec4 origin;
    dvec4 dir;
    PhongMaterial* mat;

    ray(dvec4 o, dvec4 d): origin(o), dir(d), mat(PhongMaterial::Air){}
    ray(dvec3 o, dvec3 d): origin({o.x, o.y, o.z, 1}), dir(d.x, d.y, d.z, 0), mat(PhongMaterial::Air){}
    ray(const ray &r): origin(r.origin), dir(r.dir), mat(PhongMaterial::Air){}

};