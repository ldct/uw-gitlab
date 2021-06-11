// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class ProceduralPhong : public Material {
public:
  ProceduralPhong(
    const glm::vec3&, const glm::vec3&, double,
    const glm::vec3&, const glm::vec3&, double,
    float, int
  );
  virtual ~ProceduralPhong();
  glm::vec3 get_colour(const std::list<Light *>&, glm::vec3&, glm::vec3&, const SceneNode*, const glm::vec3&, const glm::vec3& ambiant) const override;


private:
  glm::vec3 a_kd;
  glm::vec3 a_ks;
  double a_shininess;
  glm::vec3 b_kd;
  glm::vec3 b_ks;
  double b_shininess;
  float bump_factor;
  int blend_octaves;
};
