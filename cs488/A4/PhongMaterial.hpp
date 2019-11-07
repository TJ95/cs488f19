// Fall 2019

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  // additional ctors to create phong mats with new attributes
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref_ind);
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, 
                double ref_ind, double absorb); 
  virtual ~PhongMaterial();
  // overloading the = operator for both direct access and pointer calls
  PhongMaterial &operator=(const PhongMaterial &other);
  PhongMaterial &operator=(PhongMaterial other);

public:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  //additionals
  double refractive_index;
  double light_absorbtion;
  double m_shininess;

  static PhongMaterial* Air;
};
