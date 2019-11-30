// Fall 2019

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Texture.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include <thread>
#include <future>
#include <chrono>

#define ANTIALIASING

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);

class Hit {
public:
	bool hit;
	glm::dvec3 color;
};

glm::dvec4 world_coords(double x, double y, const glm::dmat4 &trans);
glm::dmat4 compute_device_world_transformation (double x, double y
										  ,double w, double h, double d
										  ,glm::vec3 eye, glm::vec3 view, glm::vec3 up);
ray castRay(glm::dvec4 p0, glm::dvec4 p1);
glm::dvec3 directLight(const std::list<Light*> & lights, const intersection & int_primary, int counter);
glm::dvec3 backgroundColor(int x, int y);
Hit compute_ray_color(const ray & r, const std::list<Light*> & lights, int counter);
void printProgress (float percentage);

ray refract(const ray &ray, const intersection &inter);
double fresneleffect(const ray &r, const intersection &inter);