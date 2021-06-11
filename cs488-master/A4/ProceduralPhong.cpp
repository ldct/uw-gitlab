// Winter 2020

#include "ProceduralPhong.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <iostream>

using namespace std;
using namespace glm;

// oh god
float fbm2(vec2, int);
float fbm3(vec3, int);

ProceduralPhong::ProceduralPhong(
	const glm::vec3& a_kd, const glm::vec3& a_ks, double a_shininess,
	const glm::vec3& b_kd, const glm::vec3& b_ks, double b_shininess,
	float bump_factor, int blend_octaves
) : a_kd(a_kd)
  , a_ks(a_ks)
  , a_shininess(a_shininess)
  , b_kd(b_kd)
  , b_ks(b_ks)
  , b_shininess(b_shininess)
  , bump_factor(bump_factor)
  , blend_octaves(blend_octaves) {}

ProceduralPhong::~ProceduralPhong() {}

constexpr float grad_step = 0.001;
vec3 gradient(vec3 p, int octaves) {
    return normalize(vec3(
        fbm3(vec3(p.x + grad_step, p.y, p.z), octaves) - fbm3(vec3(p.x - grad_step, p.y, p.z), octaves),
        fbm3(vec3(p.x, p.y + grad_step, p.z), octaves) - fbm3(vec3(p.x, p.y - grad_step, p.z), octaves),
        fbm3(vec3(p.x, p.y, p.z  + grad_step), octaves) - fbm3(vec3(p.x, p.y, p.z - grad_step), octaves)
    ));
}

float pattern(vec3 p, int octaves){
    vec3 q = vec3(
		fbm3( p + vec3(0.0,0.0,0.0), octaves ),
        fbm3( p + vec3(5.2,1.3,2.2), octaves ),
		fbm3( p + vec3(9.7,2.3,7.7), octaves )
	);

    vec3 r = vec3(
		fbm3(p + 4.0f*q + vec3(1.7,9.2,8.8), octaves ),
        fbm3(p + 4.0f*q + vec3(8.3,2.8,3.4), octaves ),
        fbm3(p + 4.0f*q + vec3(2.3,15.8,33.4), octaves )
	);

    return fbm3( p + 4.0f*r, octaves );
}

glm::vec3 ProceduralPhong::get_colour(const std::list<Light *>& lights, glm::vec3& normal, glm::vec3& p, const SceneNode* obstructions, const glm::vec3& eye, const vec3& ambient) const {

	assert(bump_factor < 1.0);
	vec3 perturbed_normal = normalize(normal + bump_factor*gradient(normal, 8));
	float mix_factor;
	if (blend_octaves > 50) {
		mix_factor = pattern(5.0f*normal, blend_octaves);
	} else {
		mix_factor = pattern(normal, blend_octaves);
	}
	mix_factor = glm::clamp(mix_factor, 0.0f, 1.0f);

	auto ret = vec3();

	auto kd = mix_factor * a_kd + (1-mix_factor)*b_kd;
	auto ks = mix_factor * a_ks + (1-mix_factor)*b_ks;
	auto shininess = mix_factor * a_shininess + (1-mix_factor)*b_shininess;

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
		float n_dot_l = std::max(dot(perturbed_normal, l), 0.0f);
		vec3 diffuse = kd * n_dot_l;

		vec3 h = normalize(v + l);
		float n_dot_h = std::max(dot(perturbed_normal, h), 0.0f);
		vec3 specular;

		if (n_dot_h > 0.0) {
			vec3 h = normalize(v + l);

			float s = pow(n_dot_h, shininess);
			specular.x = ks.x * s;
			specular.y = ks.y * s;
			specular.z = ks.z * s;
		}

    	ret += (diffuse+specular)*light->colour*light->attenuation(r);
	}

	return ambient*kd+ret;
}
