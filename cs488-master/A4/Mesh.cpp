// Winter 2020

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

using namespace std;
using namespace glm;

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
	, is_plane(false)
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;
	double bounding_radius = 0.0;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			bounding_radius = glm::max(bounding_radius, vx);
			bounding_radius = glm::max(bounding_radius, vy);
			bounding_radius = glm::max(bounding_radius, vz);
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	cout << "created mesh " << fname << " with bounding radius " << bounding_radius << endl;
	m_hitbox = new NonhierSphere(vec3(0.0), bounding_radius*1.1);

	if (fname == "plane.obj") {
		cout << "is a plane" << endl;
		is_plane = true;
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*

  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
float intersects_triangle(const vec3& ro, const vec3& rd, const Mesh& m, Triangle triangle) {
	float a,f,u,v;

	auto v0 = m.m_vertices[triangle.v1];
	auto v1 = m.m_vertices[triangle.v2];
	auto v2 = m.m_vertices[triangle.v3];

	auto edge1 = v1 - v0;
	auto edge2 = v2 - v0;

	vec3 h = glm::cross(rd, edge2);
	a = glm::dot(edge1, h);

	if (abs(a) < 0.001) return -1.0f;
	f = 1.0/a;
	auto s = ro - v0;
	u = f * glm::dot(s, h);

	if (u < 0.0 || u > 1.0) return -1.0f;

	auto q = glm::cross(s, edge1);

	v = f * glm::dot(rd, q);

	if (v < 0.0 || u + v > 1.0) return -1.0f;

	float t = f * glm::dot(edge2, q);

	if (t > 0.0) return t;

	return -1.0f;
}

constexpr bool RENDER_BOUNDING_BOX = false;

float Mesh::intersects(const vec3& ro, const vec3& rd, glm::vec3* out_normal) const {

	vector<float> intersections;
	map<float, vec3> normals;

	if (RENDER_BOUNDING_BOX && !is_plane) {
		return m_hitbox->intersects(ro, rd, out_normal);
	}

	if (m_hitbox->intersects(ro, rd, nullptr) < 0.0) {
		return -1.0f;
	}

	for (const Triangle triangle : m_faces) {
		float r = intersects_triangle(ro, rd, *this, triangle);
		if (r > 0.0) {

			auto v0 = m_vertices[triangle.v1];
			auto v1 = m_vertices[triangle.v2];
			auto v2 = m_vertices[triangle.v3];

			auto edge1 = v1 - v0;
			auto edge2 = v2 - v0;

			normals[r] = normalize(cross(edge1, edge2));
			intersections.push_back(r);
		}
	}
	if (intersections.size() == 0) return -1.0f;
	sort(intersections.begin(), intersections.end());
	if (out_normal) {
		*out_normal = normals[intersections[0]];
	}
	return intersections[0];
}
