#pragma once

#include <stdio.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

enum TXID
{
    TXID_IMAGE = 0
};

class Texture {
public:
    int tid;
    Texture(int id);
    virtual glm::dvec3 color(double u, double v);
};

class ImageLayer : public Texture {
public:
    ImageLayer(std::string fname);
    virtual glm::dvec3 color(double u, double v);
    
    std::vector<unsigned char> image;
    unsigned width;
    unsigned height;
};
