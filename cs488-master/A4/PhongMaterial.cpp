// Winter 2020

#include "PhongMaterial.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <iostream>

using namespace std;
using namespace glm;

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess) {}

PhongMaterial::~PhongMaterial() {}

glm::vec3 PhongMaterial::get_colour(const std::list<Light *>& lights, glm::vec3& normal, glm::vec3& p, const SceneNode* obstructions, const glm::vec3& eye, const vec3& ambient) const {

	auto ret = vec3();

	for (auto const& light : lights) {

		float r = glm::length(light->position - p);
		vec3 l = normalize(light->position - p);

		// shadow calculation
		bool any_hit = false;
		if (obstructions->intersects(p + 0.01f*normalize(l), l, nullptr, nullptr) > 0.0) {
			any_hit = true;
		}
		if (any_hit) {
			continue;
		}

		vec3 v = normalize(eye-p);
		float n_dot_l = std::max(dot(normalize(normal), l), 0.0f);
		vec3 diffuse = m_kd * n_dot_l;

		vec3 h = normalize(v + l);
		float n_dot_h = std::max(dot(normalize(normal), h), 0.0f);
		vec3 specular;

		if (n_dot_h > 0.0) {
			vec3 h = normalize(v + l);
			float n_dot_h = std::max(dot(normalize(normal), h), 0.0f);

			float s = pow(n_dot_h, m_shininess);
			specular.x = m_ks.x * s;
			specular.y = m_ks.y * s;
			specular.z = m_ks.z * s;
		}

    	ret += (diffuse+specular)*light->colour*light->attenuation(r);
	}

	return ambient*m_kd+ret;
}
