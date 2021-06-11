// Winter 2020

#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include "cs488-framework/MathUtils.hpp"
#include "cs488-framework/ShaderException.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = rot_matrix * trans;
}

void SceneNode::preRotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = trans * rot_matrix;
}

void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
}

void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
}

int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

void SceneNode::render(
		std::stack<glm::mat4>& trans_stack,
		glm::mat4& view,
		ShaderProgram& shader,
		BatchInfoMap& batchInfoMap,
		std::set<int>& selected,
		int mode
) const {
	auto tos = trans_stack.top();
	tos = tos * this->get_transform();
	trans_stack.push(tos);

	for (const SceneNode * node : this->children) {
		node->render(trans_stack, view, shader, batchInfoMap, selected, mode);
	}

	trans_stack.pop();
}


void SceneNode::collect_joints(
        std::vector<JointNode*>& out
) {
	for (SceneNode * node : this->children) {
		node->collect_joints(out);
	}
}

void SceneNode::render_shading(
		std::stack<glm::mat4>& trans_stack,
		glm::mat4& view,
		ShaderProgram& shader
) const {
	auto tos = trans_stack.top();
	tos = tos * this->get_transform();
	trans_stack.push(tos);

	for (const SceneNode * node : this->children) {
		node->render_shading(trans_stack, view, shader);
	}

	trans_stack.pop();
}

std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}
