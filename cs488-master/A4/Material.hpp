// Winter 2020

#pragma once

#include <list>

#include <glm/glm.hpp>
#include "Light.hpp"

class SceneNode;

class Material {
public:
  virtual ~Material();
  virtual glm::vec3 get_colour(const std::list<Light*>&, glm::vec3& normal, glm::vec3& p, const SceneNode* obstructions, const glm::vec3& eye, const glm::vec3& ambiant) const;

protected:
  Material();
};
