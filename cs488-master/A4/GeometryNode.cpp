// Winter 2020

#include "GeometryNode.hpp"

#include <iostream>

using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat ) {
	m_material = mat;
}

float GeometryNode::intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal, Material** out_material) const {
	auto trans_inv = glm::inverse(trans);

	vec3 trans_ro = vec3(trans_inv * vec4(ro, 1.0));
	vec3 trans_rd = vec3(trans_inv * vec4(rd, 0.0));

    auto ret = m_primitive->intersects(trans_ro, trans_rd, out_normal); 
	if (ret > 0.0 && out_material) {
		*out_material = m_material;
	}
	if (out_normal) {
		*out_normal = vec3(transpose(trans_inv)*vec4(*out_normal, 0.0));
	}
	return ret;
}
