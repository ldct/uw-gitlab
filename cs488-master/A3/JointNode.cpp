// Winter 2020

#include "JointNode.hpp"

#include <memory>

#include "cs488-framework/MathUtils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}

void JointNode::recalcTrans() {
	auto s1 = glm::rotate(float(degreesToRadians(curr_joint_x)), vec3(1,0,0));
	auto s2 = glm::rotate(float(degreesToRadians(curr_joint_y)), vec3(0,1,0));
	this->set_transform(
		s2*s1
	);
}

void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;

	set_joint_x(init);
}

void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;

	set_joint_y(init);
}

void JointNode::set_joint_x(double val) {
	curr_joint_x = glm::clamp(val, m_joint_x.min, m_joint_x.max);
	recalcTrans();
}

void JointNode::set_joint_y(double val) {
	curr_joint_y = glm::clamp(val, m_joint_y.min, m_joint_y.max);
	recalcTrans();
}

void JointNode::collect_joints(
        std::vector<JointNode*>& out
) {
	for (SceneNode * node : this->children) {
		node->collect_joints(out);
	}
	out.push_back((JointNode*)this);
}

void JointNode::reset() {
	set_joint_x(m_joint_x.init);
	set_joint_y(m_joint_y.init);
}
