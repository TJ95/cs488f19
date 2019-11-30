// Fall 2019

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  // additional ctors to create phong mats with new attributes
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref_ind);
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, double ref_ind, const int trans);
  PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, 
                double ref_ind, double extinct);
  PhongMaterial(const glm::vec3 &kd,
                const glm::vec3 &ks,
                double shininess,
                double refractive_ind,
                double extinct_coeff,
                const glm::vec4 &glossy
  );
  virtual ~PhongMaterial();
  // overloading the = operator for both direct access and pointer calls
  PhongMaterial &operator=(const PhongMaterial &other);
  PhongMaterial &operator=(PhongMaterial other);

public:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  //additionals
  double refractive_index;
  double extinction_coefficient;
  double m_shininess;
  int transparency;
  glm::dvec4 glossy_coef;

  static PhongMaterial* Air;
  // static PhongMaterial* Glass;
  // static PhongMaterial* Water;
};
