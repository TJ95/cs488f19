// Fall 2019

#include "JointNode.hpp"

using namespace glm;
#include <algorithm>

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
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

//---------------------------------------------------------------------------------------
void JointNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			if (xangle + angle > m_joint_x.max) {
				xangle = m_joint_x.max - angle;
			} else if (xangle+angle < m_joint_x.min) {
				xangle = m_joint_x.min + angle;
			} else {
				xangle = angle;
			}
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
			if (yangle + angle > m_joint_y.max) {
				yangle = m_joint_y.max - angle;
			} else if (yangle + angle < m_joint_y.min) {
				yangle = m_joint_y.min + angle;
			} else {
				yangle = angle;
			}
			
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	update_transformation();
}

char JointNode::get_rot_axis(){return 'x';}

//---------------------------------------------------------------------------------------
void JointNode::update_transformation()
{
	if (get_rot_axis() == 'x') {
		trans *= glm::rotate(degreesToRadians(xangle), glm::vec3(1,0,0));
	} else if (get_rot_axis() == 'y') {
		trans *= glm::rotate(degreesToRadians(yangle), glm::vec3(0,1,0));
	}
}