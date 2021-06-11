// Winter 2020

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <list>
#include <string>
#include <iostream>
#include <stack>
#include <set>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class JointNode;

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();

	int totalSceneNodes() const;

    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;

    void set_transform(const glm::mat4& m);

    void add_child(SceneNode* child);

    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    void preRotate(char axis, float angle);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;

    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    virtual void render(
        std::stack<glm::mat4>& trans_stack,
        glm::mat4& view,
        ShaderProgram& shader,
        BatchInfoMap& batchInfoMap,
        std::set<int>& selected,
        int mode
    ) const;

    virtual void render_shading(
        std::stack<glm::mat4>& trans_stack,
        glm::mat4& view,
        ShaderProgram& shader
    ) const;

    virtual void collect_joints(
        std::vector<JointNode*>& out
    );


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
