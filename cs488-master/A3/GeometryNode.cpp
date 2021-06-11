// Winter 2020

#include "GeometryNode.hpp"

#include <glm/glm.hpp>

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "cs488-framework/ShaderException.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <stack>

using namespace std;
using namespace glm;

static void updateShaderUniforms(
		const ShaderProgram& shader,
		const GeometryNode& node,
		const glm::mat4& viewMatrix,
		const glm::mat4& modelMatrix,
		int mode
) {
	mat4 modelView = viewMatrix * modelMatrix;
	glUniformMatrix4fv(shader.getUniformLocation("ModelView"), 1, GL_FALSE, value_ptr(modelView));

	mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
	glUniformMatrix3fv(shader.getUniformLocation("NormalMatrix"), 1, GL_FALSE, value_ptr(normalMatrix));

	if (mode == 0 /*PICK_MODE*/) {
		int idx = node.m_nodeId;
		float r = float(idx&0xff) / 255.0f;
		float g = float((idx>>8)&0xff) / 255.0f;
		float b = float((idx>>16)&0xff) / 255.0f;

		glUniform3f( shader.getUniformLocation("material.kd"), r, g, b );
		CHECK_GL_ERRORS;
	} else {
		vec3 kd = node.material.kd;
		glUniform3fv(shader.getUniformLocation("material.kd"), 1, value_ptr(kd));
	}

	vec3 ks = node.material.ks;
	glUniform3fv(shader.getUniformLocation("material.ks"), 1, value_ptr(ks));

	float shininess = node.material.shininess;
	glUniform1f(shader.getUniformLocation("material.shininess"), shininess);

	CHECK_GL_ERRORS;
}

GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId),
	  m_aux(-1.0)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::render(
		std::stack<glm::mat4>& trans_stack,
		glm::mat4& view,
		ShaderProgram& shader,
		BatchInfoMap& batchInfoMap,
		std::set<int>& selected,
		int mode
) const {
	auto tos = trans_stack.top();
	tos = tos * this->trans;
	trans_stack.push(tos);

	for (const SceneNode * node : this->children) {
		node->render(trans_stack, view, shader, batchInfoMap, selected, mode);
	}

	if (!(mode == 1 /* OVERLAY_MODE */ && (selected.count(this->m_nodeId) == 0))) {
		updateShaderUniforms(shader, *this, view, trans_stack.top(), mode);

		BatchInfo batchInfo = batchInfoMap[this->meshId];

		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);

	}

	trans_stack.pop();
}

static void updateShaderUniforms_shading(
	const string node_name,
	const ShaderProgram& shader,
	const glm::mat4& trans,
	const float aux
) {
	auto new_x = trans * glm::vec4(1.0, 0.0, 0.0, 0.0);
	auto new_y = trans * glm::vec4(0.0, 1.0, 0.0, 0.0);
	auto new_z = trans * glm::vec4(0.0, 0.0, 1.0, 0.0);

	glUniform3f(
		shader.getUniformLocation((node_name + "_scale").c_str()),
		length(new_x), length(new_y), length(new_z)
	);

	new_x = normalize(new_x);
	new_y = normalize(new_y);
	new_z = normalize(new_z);

	auto T = mat3(
		new_x.x, new_x.y, new_x.z,
		new_y.x, new_y.y, new_y.z,
		new_z.x, new_z.y, new_z.z
	);

	T = inverse(T);

	glUniformMatrix3fv(
		shader.getUniformLocation((node_name + "_rotinv").c_str()),
		1, GL_FALSE,
		&T[0][0]
	);

	auto pos = trans * glm::vec4(0.0, 0.0, 0.0, 1.0);

	glUniform3f(
		shader.getUniformLocation((node_name + "_pos").c_str()),
		pos.x, pos.y, pos.z
	);

	if (aux > 0) {
		glUniform1f(
			shader.getUniformLocation((node_name + "_aux").c_str()),
			aux
		);
	}
}

void GeometryNode::render_shading(
		std::stack<glm::mat4>& trans_stack,
		glm::mat4& view,
		ShaderProgram& shader
) const {
	auto tos = trans_stack.top();
	tos = tos * this->trans;
	trans_stack.push(tos);

	for (const SceneNode * node : this->children) {
		node->render_shading(trans_stack, view, shader);
	}

	updateShaderUniforms_shading(this->m_name, shader, trans_stack.top(), this->m_aux);


	trans_stack.pop();
}
