// Winter 2020

#include "Material.hpp"

#include <iostream>

using namespace std;

Material::Material()
{}

Material::~Material()
{}

glm::vec3 Material::get_colour(const std::list<Light *>& lights, glm::vec3& normal, glm::vec3& p, const SceneNode* obstructions, const glm::vec3& eye, const glm::vec3& ambiant) const {
    return glm::vec3(0.5, 0.0, 0.5); // magenta
}
