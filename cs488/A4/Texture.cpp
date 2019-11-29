#pragma once

using namespace glm;

#include "Texture.hpp"
//#include <glm/gtc/noise.hpp>
#include <lodepng/lodepng.h>
#include <iostream>

Texture::Texture(int id) : textureID(id) { };

dvec3 Texture::color(double u, double v) {
    return dvec3{0, 0, 0};
}

ImageLayer::ImageLayer(std::string fname) : Texture(TXID_IMAGE) {
    unsigned error = lodepng::decode(image, width, height, filename);
    if(error) std::cout << "could not lode image " << error << ": " << lodepng_error_text(error) << std::endl;
}

dvec3 ImageLayer::color(double u, double v) {
    if (std::abs(u) > 1 || std::abs(v) >1) {
        cout << "error in image color" << endl;
        return {0, 0, 0};
    }
    int pixel_x = (u + 1) / 2 * (width - 1);
    int pixel_y = (v + 1) / 2 * (height - 1);
    
    unsigned long position = pixel_y * width + pixel_x;
    position = position * 4;
    
    return glm::dvec3{image[position] / 255.0, image[position + 1] / 255.0, image[position + 2] / 255.0};
}