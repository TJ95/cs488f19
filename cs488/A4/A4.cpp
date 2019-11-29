// Fall 2019

#include <glm/ext.hpp>
using namespace glm;

#include "A4.hpp"
using namespace std;

#define DISTANCE 10.0
#define MAX_RECURSE 5
#define EPSILON 0.0001
#include <glm/gtx/io.hpp>

static SceneNode *Scene;
static glm::vec3 aColor;
static double IMAGEWIDTH;
static double IMAGEHEIGHT;

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
) {

  // Fill in raytracing code here...  

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	Scene = root;
	aColor = ambient;
	auto colorMax = 1.0;
	auto colorMin = 0.0;

	size_t h = image.height();
	IMAGEHEIGHT = h;
	size_t w = image.width();
	IMAGEWIDTH = w;

	double xi = 2 * DISTANCE * tan(radians(fovy / 2));
	double yi = 2 * DISTANCE * tan(radians(fovy / 2));

	auto big_trans = compute_device_world_transformation(w, h, xi, yi, DISTANCE, eye, view, up);
	
	// if (ANTIALIASING) {
#ifdef ANTIALIASING
	cout << "!anti-aliasing active!" << endl;
#endif
	// }
	// rendering code
	for (uint y = 0; y < h; ++y) {
		// std::cout << "Currently Handling Row: " << y << std::endl;
		// progress bar display
		for (uint x = 0; x < w; ++x) {
			printf("\r[%3d%%]", (int) ((y * h + x) * 100 / (1.0 * w * h)));
#ifdef ANTIALIASING
			//if (ANTIALIASING)
			//{
				auto p_world =  world_coords(x, y, big_trans);
				auto p_world1 = world_coords(x + 1, y, big_trans);
				auto p_world2 = world_coords(x - 1, y, big_trans);
				auto p_world3 = world_coords(x, y + 1, big_trans);
				auto p_world4 = world_coords(x, y - 1, big_trans);

				p_world1 = 0.5 * p_world + 0.5 * p_world1;
				p_world2 = 0.5 * p_world + 0.5 * p_world2;
				p_world3 = 0.5 * p_world + 0.5 * p_world3;
				p_world4 = 0.5 * p_world + 0.5 * p_world4;

				ray r =  castRay(glm::dvec4(eye, 1), p_world);
				ray r1 = castRay(glm::dvec4(eye, 1), p_world1);
				ray r2 = castRay(glm::dvec4(eye, 1), p_world2);
				ray r3 = castRay(glm::dvec4(eye, 1), p_world3);
				ray r4 = castRay(glm::dvec4(eye, 1), p_world4);

				glm::dvec3 color(0, 0, 0);
				glm::dvec3 color1(0, 0, 0);
				glm::dvec3 color2(0, 0, 0);
				glm::dvec3 color3(0, 0, 0);
				glm::dvec3 color4(0, 0, 0);

				Hit hc =  compute_ray_color(r, lights, 0);
				Hit hc1 = compute_ray_color(r1, lights, 0);
				Hit hc2 = compute_ray_color(r2, lights, 0);
				Hit hc3 = compute_ray_color(r3, lights, 0);
				Hit hc4 = compute_ray_color(r4, lights, 0);

				// averge all of them
				if (hc.hit)
				{
					color += glm::clamp(hc.color, colorMin, colorMax);
				}
				else
				{
					color += backgroundColor(x, y);
				}

				if (hc1.hit)
				{
					color += glm::clamp(hc1.color, colorMin, colorMax);
				}
				else
				{
					color += backgroundColor(x, y);
				}

				if (hc2.hit)
				{
					color += glm::clamp(hc2.color, colorMin, colorMax);
				}
				else
				{
					color += backgroundColor(x, y);
				}
				if (hc3.hit)
				{
					color += glm::clamp(hc3.color, colorMin, colorMax);
				}
				else
				{
					color += backgroundColor(x, y);
				}
				if (hc4.hit)
				{
					color += glm::clamp(hc4.color, colorMin, colorMax);
				}
				else
				{
					color += backgroundColor(x, y);
				}

				color = color / 5.0; // average the color.

				image(x, y, 0) = color.r;
				image(x, y, 1) = color.g;
				image(x, y, 2) = color.b;
#else			//} else {
				auto p_world = world_coords(x, y, big_trans);
				ray r = castRay(dvec4(eye, 1), p_world);
				glm::dvec3 color(0, 0, 0);
				Hit hc = compute_ray_color(r, lights, 0);
				if (hc.hit) {
					image(x, y, 0) = glm::clamp(hc.color.r, colorMin, colorMax);
					image(x, y, 1) = glm::clamp(hc.color.g, colorMin, colorMax);
					image(x, y, 2) = glm::clamp(hc.color.b, colorMin, colorMax);
				} else {
					glm::dvec3 color = backgroundColor(x, y);
					image(x, y, 0) = color.r; //red
					image(x, y, 1) = color.g; //green
					image(x, y, 2) = color.b; //blue
				//}
				}
#endif
		}
	}
}

dmat4 compute_device_world_transformation (double w, double h
										  ,double xi, double yi, double d
										  ,vec3 eye, vec3 view, vec3 up) {
	// first translation to move to the centre
	auto transC = translate(dmat4(), dvec3(-w/2, -h/2, d));
	// scaling with respect to the image ratio
	auto scal = scale(dmat4(), dvec3(-yi/h, xi/w, 1));
	// rotation
	vec3 wr = view;
	wr = normalize(wr);
	up = normalize(up);
	vec3 u = glm::cross(up, wr);
	u = normalize(u);
	vec3 v = glm::cross(u, wr);

	dmat4 rot({{u.x, u.y, u.z, 0},
			  {v.x, v.y, v.z, 0},
			  {wr.x, wr.y, wr.z, 0},
			  {0, 0, 0, 1}});
	// second translation
	dmat4 trans2({{1, 0, 0, 0},
				  {0, 1, 0, 0},
				  {0, 0, 1, 0},
				  {eye.x, eye.y, eye.z, 1}});
	
	return trans2 * rot * scal * transC;
}

dvec4 world_coords(double x, double y, dmat4 &trans)
{
	auto device_coords = dvec4(x, y, 0, 1);
	auto cd_w = trans * device_coords;
	return cd_w;
}

ray castRay(dvec4 p0, dvec4 p1) {
	ray r = ray(p0, p1 - p0);
	return r;
}

intersection hit_detection(const ray & r, SceneNode * root) {
	std::list<mat4> transformations;
	auto result = root->intersect(r, transformations);
	return result;
}

dvec3 directLight(const std::list<Light*> & lights, const intersection & int_primary, int counter) {
	glm::vec3 color(0, 0, 0);

	if (counter > MAX_RECURSE) {
		return color;
	}

	glm::dvec4 point = int_primary.received_ray.origin + int_primary.received_ray.dir * int_primary.t;
if (int_primary.mat->refractive_index != 0) {
	// reflectance factor
	//double reflectance = 0.3;
	double reflectance = fresneleffect(int_primary.received_ray, int_primary);

	glm::dvec3 Ri = glm::normalize(glm::dvec3(int_primary.received_ray.dir));
	glm::dvec3 N = glm::normalize(glm::dvec3(int_primary.normal));
	glm::dvec3 Rr = Ri - 2.0 * N * (glm::dot(Ri, N));

	dvec4 Rr_dir = dvec4(glm::normalize(Rr), 0);
	ray reflected(point + EPSILON * Rr_dir, Rr_dir);
	Hit hc = compute_ray_color(reflected, lights, counter + 1);

	if (reflectance != 1.0) {
		ray t = refract(int_primary.received_ray, int_primary);
		Hit refractedColor = compute_ray_color(t, lights, counter + 1); // get refracted ray color.
		if (refractedColor.hit){
			color += (1 - reflectance) * refractedColor.color;
		}
	}
	color += reflectance * int_primary.mat->m_ks * glm::vec3(hc.color);
}
	glm::dvec3 Ri = glm::normalize(glm::dvec3(int_primary.received_ray.dir));
	glm::dvec3 N = glm::normalize(glm::dvec3(int_primary.normal));
	glm::dvec3 Rr = Ri - 2.0 * N * (glm::dot(Ri, N));
	// calculate diffused color
	for (auto light : lights) {
		glm::dvec3 colorSum {0, 0, 0};
		glm::dvec4 point = int_primary.received_ray.origin + int_primary.received_ray.dir * int_primary.t;
		glm::dvec4 light_direction = glm::dvec4(light->position, 1) - point;
		
		// shadow
		auto lightPosition =  light->position;			
		glm::dvec4 shadow_ray_direction = glm::dvec4(lightPosition, 1) - point;
		ray shadow_ray = ray(point + EPSILON * glm::normalize(shadow_ray_direction), shadow_ray_direction);
			
		double shadow_ray_length = glm::length(shadow_ray_direction);
			
		intersection shadow_intersect = hit_detection(shadow_ray, Scene);
		
		if (shadow_intersect.hit && glm::length(shadow_ray_direction * shadow_intersect.t) < shadow_ray_length) {

		} else {
			auto light_ray = glm::normalize(shadow_ray_direction);
			auto normal = glm::normalize(int_primary.normal);
			auto cosineTheta = std::max(glm::dot(glm::dvec3(normal), glm::dvec3(light_ray)), 0.0);
			auto kd = int_primary.mat->m_kd;	

			if (glm::length(kd) != 0) {
				// length of the light
				double length = int_primary.t * glm::length(int_primary.received_ray.dir);
				// Calculate diffused color
				colorSum += kd * cosineTheta * light->colour / (light->falloff[0] +
																light->falloff[1] * length +
																light->falloff[2] * length * length);	
			}	
			if (glm::length(int_primary.mat->m_ks) > 0) {
				cosineTheta = std::max(glm::dot(Rr, glm::normalize(glm::dvec3(light_direction))), 0.0);
				auto phongCoeff = std::pow(cosineTheta, int_primary.mat->m_shininess);	
				colorSum += phongCoeff * int_primary.mat->m_ks * light->colour;
			}
		}
		color += colorSum;
	}

	//color += reflectance * int_primary.mat->m_ks * glm::vec3(hc.color);

	return color;
}

Hit compute_ray_color(const ray & r, const std::list<Light*> & lights, int counter) {
	dvec3 color(0, 0, 0);

	if (counter > MAX_RECURSE) {
		return {false, color};
	}

	intersection primary_intersect = hit_detection(r, Scene);
	
	if (primary_intersect.hit) {
        // ambient color
		color += primary_intersect.mat->m_kd * aColor;	
		// calculate the ray color
		color += directLight(lights, primary_intersect, counter);
	}
	return Hit{primary_intersect.hit, color};
}

dvec3 backgroundColor(int x, int y)
{	
	glm::dvec3 color(0, 0, 0);
	color.r += 0;
	color.g += 0;
	color.b += (double)y / IMAGEHEIGHT;
	return color;
}

// Compute the direction for a refracted ray and return a newly generated ray object
ray refract(const ray& r, const intersection& inter) {

	dvec4 I = glm::normalize(r.dir);
	dvec4 N = glm::normalize(inter.normal);
	//auto ior = inter.mat->refractive_index;
	//cout << "ior:" << ior << endl;

	double NdotI = glm::dot(N,I);
	double etai = inter.from_mat->refractive_index;
	double etat = inter.mat->refractive_index;

	double cosi = NdotI;
	//clamping
	if (cosi > 1) cosi = 1;
	if (cosi < -1) cosi = -1;
	auto n = N;

	if (cosi < 0) {
		cosi = -cosi;
	} else {
		std::swap(etai, etat);
		n = -N;
	}
	double eta = etai/etat;
	double k = 1 - eta * eta * (1 - cosi * cosi);
	dvec4 direction;
	if (k < 0) {
		//cout << "Total internal refraction" << endl;
		return ray({0, 0, 0, 1}, {0, 0, 0, 0});
	} else {
		glm::dvec4 hitpoint = r.origin + r.dir * inter.t;
		direction = eta * I + (eta * cosi - sqrt(k)) * n;
		direction = glm::normalize(direction);
		ray new_ray(hitpoint + EPSILON * direction, direction);
		return new_ray;
	}
}

//simulate the fresnel effect
double fresneleffect(const ray &r, const intersection &inter){

	double kr;
	dvec4 I = glm::normalize(r.dir);
	dvec4 N = glm::normalize(inter.normal);
	//cout << "ior:" << ior << endl;

	double NdotI = glm::dot(N, I);
	double etai = inter.from_mat->refractive_index;
	double etat = inter.mat->refractive_index;

	double cosi = NdotI;
	//clamping
	if (cosi > 1) cosi = 1;
	if (cosi < -1) cosi = -1;

	if (cosi > 0) std::swap(etai, etat);
	// compute sini using snell's law
	double sint = etai/etat * sqrt(std::max(0.0, 1- cosi * cosi));
	// total internal refraction
	if (sint >= 1) {
		kr = 1;
	} else {
		double cost = sqrt(std::max(0.0, 1 - sint * sint));
		cosi = abs(cosi);
		double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	return kr;
}

// simulate the fresnel effect
// double fresneleffect(const ray &r, const intersection &inter)
// {

// 	float kr;
// 	dvec4 I = glm::normalize(r.dir);
// 	dvec4 N = glm::normalize(inter.normal);
// 	//cout << "ior:" << ior << endl;

// 	float NdotI = glm::dot(N, I);
// 	float etai = inter.from_mat->refractive_index;
// 	float etat = inter.mat->refractive_index;

// 	float cosi = NdotI;
// 	//clamping
// 	if (cosi > 1)
// 		cosi = 1;
// 	if (cosi < -1)
// 		cosi = -1;

// 	if (cosi > 0)
// 		std::swap(etai, etat);
// 	// compute sini using snell's law
// 	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
// 	// total internal refraction
// 	if (sint >= 1)
// 	{
// 		kr = 1;
// 	}
// 	else
// 	{
// 		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
// 		cosi = fabs(cosi);
// 		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
// 		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
// 		kr = (Rs * Rs + Rp * Rp) / 2;
// 	}
// 	return kr;
// }