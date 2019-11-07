// Fall 2019

#include "PhongMaterial.hpp"

static PhongMaterial air_metric = PhongMaterial({0, 0, 0}, {0, 0, 0}, 0, 1.0);
PhongMaterial *PhongMaterial::Air = &air_metric;

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	refractive_index = 1.0;
	light_absorbtion = 0.0;
}

// overloading ctors
PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, refractive_index(ref)
{
	light_absorbtion = 0.0;
}

PhongMaterial::PhongMaterial(
	const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref, double absorb)
	: m_kd(kd), m_ks(ks), m_shininess(shininess), refractive_index(ref), light_absorbtion(absorb)
{}

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