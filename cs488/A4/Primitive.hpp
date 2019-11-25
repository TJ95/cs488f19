// Fall 2019

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "intersection.hpp"
#include <list>
#include <exception>

//#define EPSILON 0.0001

class Primitive {
public:
  virtual ~Primitive();
  virtual intersection intersect(const ray &r);
  virtual intersection intersect(const ray &r, std::list<glm::mat4> trans);
protected:
  Primitive *m_primitive;
};

class Sphere : public Primitive {
public:
  Sphere();
	virtual ~Sphere();
	virtual intersection intersect(const ray & r);
};

class Cube : public Primitive {
public:
  Cube();
	virtual ~Cube();
	virtual intersection intersect(const ray & ray);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {}
  virtual ~NonhierSphere();
	virtual intersection intersect(const ray & ray);
	virtual intersection intersect(const ray & ray, std::list<glm::mat4> transformations);

public:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
	NonhierBox(const glm::vec3& pos, double size)
	: m_pos(pos), m_size(size), m_pos_2(size * glm::vec3(1,1,1))
	{
	}
  virtual intersection intersect(const ray &ray);
  // virtual glm::vec4 getNormal(const glm::vec4& point);
	virtual ~NonhierBox();
private:
	glm::vec3 m_pos;
	double m_size;
	glm::vec3 m_pos_2;
};