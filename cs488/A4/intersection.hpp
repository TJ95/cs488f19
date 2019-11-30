#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

#include <glm/ext.hpp>
#include "PhongMaterial.hpp"
#include "ray.hpp"
#include "Texture.hpp"
using namespace glm;

class SceneNode; // to compile

/*
 * information of an intersection point
 * stored in a class
 */
class intersection {
public:
    // incoming ray of the intersection
    ray received_ray;

    // hit or miss
    bool hit;

    // distance to intersection point on the polygon plane
    double t;

    // normal (of the intersection pt)
    dvec4 normal;

    // materials
    PhongMaterial* mat;
    PhongMaterial* from_mat;
    SceneNode* node;
    // ctor
    intersection();
    intersection(const ray &ray, double pt, bool hit); // knowing hit/miss
    intersection(const ray &ray, double t);
    Texture *textureInfo() const;

    dvec4 primitive_int_point;
    
};