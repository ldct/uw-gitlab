// Winter 2020

#include "Primitive.hpp"

#include <iostream>

using namespace std;
using namespace glm;

float Primitive::intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal) const {
    return -1.0;
}

Primitive::~Primitive() { }

Sphere::~Sphere()
{
}

Cube::~Cube() { }

NonhierSphere::~NonhierSphere() { }

NonhierBox::~NonhierBox() { }

// http://www.cs.utah.edu/~awilliam/box/box.pdf
float NonhierBox::intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal) const {
    vec3 bounds[2];
    bounds[0] = m_pos;
    bounds[1] = m_pos;

    bounds[1].x += m_size;
    bounds[1].y += m_size;
    bounds[1].z += m_size;

    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    if (rd.x >= 0) {
        tmin = (bounds[0].x - ro.x) / rd.x;
        tmax = (bounds[1].x - ro.x) / rd.x;
    } else {
        tmin = (bounds[1].x - ro.x) / rd.x;
        tmax = (bounds[0].x - ro.x) / rd.x;
    }

    if (rd.y >= 0) {
        tymin = (bounds[0].y - ro.y) / rd.y;
        tymax = (bounds[1].y - ro.y) / rd.y;
    } else {
        tymin = (bounds[1].y - ro.y) / rd.y;
        tymax = (bounds[0].y - ro.y) / rd.y;
    }

    if ((tmin > tymax) || (tymin > tmax)) return -1.0f;

    if (tymin > tmin) tmin = tymin;

    if (tymax < tmax) tmax = tymax;

    if (rd.z >= 0) {
        tzmin = (bounds[0].z - ro.z) / rd.z;
        tzmax = (bounds[1].z - ro.z) / rd.z;
    } else {
        tzmin = (bounds[1].z - ro.z) / rd.z;
        tzmax = (bounds[0].z - ro.z) / rd.z;
    }

    if ((tmin > tzmax) || (tzmin > tmax)) return -1.0f;
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    if (tmin < 0 && tmax < 0) return -1.0f;

    float t;
    if (tmin < 0) {
        t = tmax;
    } else {
        t = tmin;
    }

    auto p = ro + t*rd;

    if (out_normal) {
        *out_normal = normal(p);
    }

    return t;
}

vec3 NonhierBox::normal(const glm::vec3& p) const {
    auto sn = (p - m_pos - vec3(m_size/2));
    auto an = vec3(abs(sn.x), abs(sn.y), abs(sn.z));

    auto max_component = glm::max(an.x, glm::max(an.y, an.z));
    if (max_component == an.x) {
        if (sn.x > 0) return vec3(1.0, 0.0, 0.0);
        return vec3(-1.0, 0.0, 0.0);
    } else if (max_component == an.y) {
        if (sn.y > 0) return vec3(0.0, 1.0, 0.0);
        return vec3(1.0, -1.0, 0.0);
    } else {
        if (sn.z > 0) return vec3(0.0, 0.0, 1.0);
        return vec3(0.0, 0.0, -1.0);
    }
}

float NonhierSphere::intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal) const {
    auto A = glm::dot(rd, rd);
    auto B = 2*glm::dot(rd, ro - m_pos);
    auto C = glm::dot(ro - m_pos, ro - m_pos) - m_radius*m_radius;

    auto d = B*B - 4*A*C;

    if (d >= 0) {
        auto t0 = (-B - sqrt(d)) / (2*A);
        auto t1 = (-B + sqrt(d)) / (2*A);

        if (t1 < 0) return -1.0;

        float t;

        if (t0 < 0) {
            t = t1;
        } else {
            t = t0;
        }
        auto p = ro + t*rd;

        if (out_normal) {
            *out_normal = (p - m_pos) * float(1.0 / m_radius);
        }

        return t;
    } else {
        return -1.0;
    }
}
