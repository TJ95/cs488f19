// Fall 2019

#include "PhongMaterial.hpp"

static PhongMaterial air_metric = PhongMaterial({0, 0, 0}, {0, 0, 0}, 0, 1.0);
// static PhongMaterial glass_metric = PhongMaterial({0, 0, 0}, {0, 0, 0}, 0, 1.5, 9.9359e-9);
// static PhongMaterial water_metric = PhongMaterial({0, 0, 0}, {0, 0, 0}, 0, 1.3, 0.000002);

PhongMaterial *PhongMaterial::Air = &air_metric;
// PhongMaterial *PhongMaterial::Glass = &glass_metric;
// PhongMaterial *PhongMaterial::Water = &water_metric;

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	refractive_index = 1.0;
	extinction_coefficient = 0.0;
	transparency = 0;
}

// overloading ctors
PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref_ind)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, refractive_index(ref_ind)
{
	extinction_coefficient = 0.0;
	transparency = 0;
}

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref_ind, const int trans)
	: m_kd(kd), m_ks(ks), m_shininess(shininess), refractive_index(ref_ind), transparency(trans)
{
	extinction_coefficient = 0.0;
}

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks,
	double shininess, double ref_ind, const int trans, const glm::vec4 &glossy)
	: m_kd(kd), m_ks(ks), m_shininess(shininess), refractive_index(ref_ind), transparency(trans)
{
	extinction_coefficient = 0.0;
	glossy_coef = glossy;
}

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref_ind, double ex)
	: m_kd(kd), m_ks(ks), m_shininess(shininess), refractive_index(ref_ind), extinction_coefficient(ex)
{
	transparency = 0;
}
// PhongMaterial::PhongMaterial(const glm::vec3 &kd,
// 			  const glm::vec3 &ks,
// 			  double shininess,
// 			  double refractive_ind,
// 			  double extinct_coeff,
// 			  const glm::vec4 &glossy) 
// {
// 	glossy_coef = glossy;
// }

// default dtor
PhongMaterial::~PhongMaterial()
{}

PhongMaterial &PhongMaterial::operator=(const PhongMaterial &other)
{
	this->m_kd = other.m_kd;
	this->m_ks = other.m_ks;
	this->m_shininess = other.m_shininess;
	this->refractive_index = other.refractive_index;
	return *this;
}

PhongMaterial &PhongMaterial::operator=(PhongMaterial other)
{
	this->m_kd = other.m_kd;
	this->m_ks = other.m_ks;
	this->m_shininess = other.m_shininess;
	this->refractive_index = other.refractive_index;
	return *this;
}