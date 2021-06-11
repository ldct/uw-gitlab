// Winter 2020

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	SceneNode* selection;

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	void set_joint_x(double val);
	void set_joint_y(double val);

    void collect_joints(
        std::vector<JointNode*>& out
    ) override;

	JointRange m_joint_x, m_joint_y;

	double curr_joint_x, curr_joint_y;

	void recalcTrans();

	void reset();
};
