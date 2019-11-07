// Fall 2019

#include "JointNode.hpp"

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
	float xangle, yangle;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;

	xangle = init;
	trans = glm::rotate(degreesToRadians(xangle), glm::vec3(1, 0, 0)) * trans;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;

	yangle = init;
	trans = glm::rotate(degreesToRadians(yangle), glm::vec3(0, 1, 0)) * trans;
}

