// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual ~PhongMaterial();
  glm::vec3 get_colour(const std::list<Light *>&, glm::vec3&, glm::vec3&, const SceneNode*, const glm::vec3&, const glm::vec3& ambiant) const override;


private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
};
