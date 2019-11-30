#include "Texture.hpp"
#include <lodepng/lodepng.h>
#include <iostream>

using namespace glm;

dvec3 Texture::color(double u, double v) {
    return dvec3{0, 0, 0};
}

ImageTexture::ImageTexture(std::string filename){
    unsigned error = lodepng::decode(image, width, height, filename);
    //std::cout << "image metric: " << width << "," << height << std::endl;
    if (error) {
        std::cout << "could not load image " << error << ": " << lodepng_error_text(error) << std::endl;
    }
}

dvec3 ImageTexture::color(double u, double v) {
    
    if (std::abs(u) > 1 || std::abs(v) >1) {
        std::cout << "error in image color" << std::endl;
        return {0, 0, 0};
    }
    int pixel_x = (u + 1) / 2 * (width - 1);
    int pixel_y = (v + 1) / 2 * (height - 1);

    unsigned long position = pixel_y * width + pixel_x;
    position = position * 4;
    return dvec3{image[position] / 255.0, image[position + 1] / 255.0, image[position + 2] / 255.0};
}