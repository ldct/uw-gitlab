// Winter 2020

#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	float m_aux;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	void render(
        std::stack<glm::mat4>& trans_stack,
        glm::mat4& view,
        ShaderProgram& shader,
		BatchInfoMap& batchInfoMap,
		std::set<int>& selected,
		int mode
    ) const override;

	void render_shading(
        std::stack<glm::mat4>& trans_stack,
        glm::mat4& view,
		ShaderProgram& shader
    ) const override;
};
