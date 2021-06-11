// Winter 2020

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

float Light::attenuation(float r) {
  return 1.0 / (falloff[0] + falloff[1]*r + falloff[2]*r*r);
}

std::ostream& operator<<(std::ostream& out, const Light& l) {
  out << "L[" << glm::to_string(l.colour)
  	  << ", " << glm::to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
