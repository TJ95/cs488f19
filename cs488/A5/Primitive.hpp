// Fall 2019

#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "intersection.hpp"
#include <list>
#include <exception>

//#define EPSILON 0.0001
enum PRIMID {
	P_PLANE = 0,
	P_CUBE = 1,
	P_SPHERE = 2,
	P_CONE = 3,
	P_CYLINDER = 4,
	P_TORUS = 5
};

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

// Create a plane
class Plane : public Primitive {
	virtual intersection intersect(const ray & ray);
	virtual bool isOpticsEnabled() {return true;};
};

// Create standard cone with Centered at Origin, H = 2 (spanning from -1 to 1), R = 1
class Cone : public Primitive {
public:
	Cone();
    virtual ~Cone();
    virtual intersection intersect(const ray & ray);
};

class Cylinder : public Primitive {
public:
    Cylinder();
    virtual ~Cylinder();
    virtual intersection intersect(const ray & ray);
    bool    isValidRoot(const ray &ray, double t);
};

class NonhierTorus : public Primitive {
public:
  NonhierTorus(const glm::dvec3& pos, double oradius, double iradius)
    : m_pos(pos)
    , m_oradius(oradius)
    , m_iradius(iradius)
  {
  }
  virtual ~NonhierTorus();
  virtual intersection intersect(const ray& ray);
private:
  glm::dvec3 m_pos;
  double m_oradius;
  double m_iradius;
};

class Torus : public Primitive {
public:
Torus();
  virtual ~Torus();

  virtual intersection intersect(const ray& ray);
};