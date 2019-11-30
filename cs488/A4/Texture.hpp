#pragma once

#include <stdio.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Texture {
public:
    int tid;
    virtual glm::dvec3 color(double u, double v);
};

class ImageTexture : public Texture {
public:
    ImageTexture(std::string filename);
    virtual glm::dvec3 color(double u, double v);
    
    std::vector<unsigned char> image;
    unsigned width;
    unsigned height;
};
