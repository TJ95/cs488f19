#pragma once

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>
#include <string>
#include <vector>

class Texture {
public:
    int tid;
    virtual glm::dvec3 color(double u, double v);
    virtual glm::dvec3 color_sphere(const glm::dvec3 center, const glm::dvec4 p);
};

class ImageTexture : public Texture {
public:
    ImageTexture(std::string filename);
    virtual glm::dvec3 color(double u, double v);
    virtual glm::dvec3 color_sphere(const glm::dvec3 center, const glm::dvec4 p);

    std::vector<unsigned char> image;
    unsigned width;
    unsigned height;
};

class Bumpmap {
public:
    Bumpmap(std::string filename);
    // virtual glm::dvec3 bumper_sphere(const glm::dvec3 n, const glm::dvec3 pu, const glm::dvec3 pv, const glm::dvec3 u, const glm::dvec3 v);
    virtual glm::dvec3 bumper_sphere(const glm::dvec3 normal, const glm::dvec3 center, const glm::dvec4 p);

    std::vector<unsigned char> map;
    unsigned width;
    unsigned height;
};
