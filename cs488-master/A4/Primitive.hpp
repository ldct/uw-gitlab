// Winter 2020

#pragma once

#include <glm/glm.hpp>

class Primitive {
public:
  virtual ~Primitive();
  virtual float intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal) const;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius) : m_pos(pos), m_radius(radius) { }
  virtual ~NonhierSphere();
  float intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal) const override;


private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size) { }

  float intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal) const override;
  glm::vec3 normal(const glm::vec3& p) const;

  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
