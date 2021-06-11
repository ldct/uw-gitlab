// Winter 2020

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim,
		Material *mat = nullptr );

	void setMaterial( Material *material );

	virtual float intersects(const glm::vec3& ro, const glm::vec3& rd, glm::vec3* out_normal, Material** out_material) const override;


	Material *m_material;
	Primitive *m_primitive;
};
