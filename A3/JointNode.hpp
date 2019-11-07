// Fall 2019

#pragma once

#include "SceneNode.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include "cs488-framework/GlErrorCheck.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "cs488-framework/MathUtils.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);
	float xangle, yangle;

	struct JointRange {
		double min, init, max;
	};


	JointRange m_joint_x, m_joint_y;
};
