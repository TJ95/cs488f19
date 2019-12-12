#include "Texture.hpp"
#include <lodepng/lodepng.h>
#include <iostream>

using namespace glm;

dvec3 Texture::color(double u, double v) {
    return dvec3{0, 0, 0};
}

dvec3 Texture::color_sphere(const glm::dvec3 center, const glm::dvec4 p) {
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

dvec3 ImageTexture::color_sphere(const dvec3 center, const dvec4 p) {

    double pi = glm::pi<double>(); 
    dvec3 pn = {p.x, p.y, p.z};
    dvec3 n = glm::normalize(pn - center);
    float u = atan2(n.x, n.z) / (2 * pi) + 0.5;
    float v = 0.5 - asin(n.y) / pi;

    int pixel_x = u * (width);
    int pixel_y = v * (height);

    unsigned long position = pixel_y * width + pixel_x;
    position = position * 4;
    return dvec3{image[position] / 255.0, image[position + 1] / 255.0, image[position + 2] / 255.0};
}

Bumpmap::Bumpmap(std::string filename) {
    unsigned error = lodepng::decode(map, width, height, filename);
    //std::cout << "image metric: " << width << "," << height << std::endl;
    if (error) {
        std::cout << "could not load image " << error << ": " << lodepng_error_text(error) << std::endl;
    }
}

// dvec3 Bumpmap::bumper_sphere(const dvec3 n, const dvec3 pu, const dvec3 pv, const dvec3 u, const dvec3 v) {

//     return dvec3{0,0,0};
// }

glm::dvec3 Bumpmap::bumper_sphere(const glm::dvec3 normal, const dvec3 center, const dvec4 p) {
    // TODO
    dvec3 t = cross(normal, {0.0,1.0,0.0});
    dvec3 b;
    int t_length = sqrt(t.x*t.x + t.y*t.y + t.z*t.z);
    if (t_length==0) t = cross(normal, {0.0,0.0,1.0});
    
    t = normalize(t);
    b = normalize(cross(normal, t));

    // getting map color
    double pi = glm::pi<double>(); 
    dvec3 pn = {p.x, p.y, p.z};
    dvec3 n = glm::normalize(pn - center);
    float u = atan2(n.x, n.z) / (2 * pi) + 0.5;
    float v = 0.5 - asin(n.y) / pi;

    int pixel_x = u * (width);
    int pixel_y = v * (height);

    unsigned long position = pixel_y * width + pixel_x;
    position = position * 4;
    dvec3 bumpmap_n = dvec3{map[position] / 255.0, map[position + 1] / 255.0, map[position + 2] / 255.0};
    // ---------------------
    bumpmap_n = normalize(bumpmap_n);
    dvec3 temp{1.0,1.0,1.0};
    bumpmap_n *= 2;
    bumpmap_n = bumpmap_n - temp;
    dmat3 tbn = dmat3(t,b,normal);
    return normalize(tbn*bumpmap_n);
}