// Fall 2019

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;
unordered_map<int, SceneNode *> SceneNode::node_bank;
bool SceneNode::do_picking = false;

//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	parent(nullptr),
	m_nodeId(nodeInstanceCount++)
{
	node_bank[m_nodeId] = this;
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
	child->parent = this;
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

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------
void SceneNode::render(
	const ShaderProgram &shader,
	const glm::mat4 &viewMatrix,
	BatchInfoMap &bim,
	std::deque<glm::mat4> &stack) const {

		// perform current transformation
		stack.push_back(trans);
		// render the children
		for (const SceneNode * node : this->children)
		{
			node->render(shader, viewMatrix, bim, stack);
		}
		// pop executed trans
		stack.pop_back();
}
//---------------------------------------------------------------------------------------
void SceneNode::updateShaderUniforms(
	const ShaderProgram &shader,
	const glm::mat4 &viewMatrix,
	std::deque<glm::mat4> &stack) const {}

void SceneNode::picking_switch(int cmd) const {
	if (cmd == 0) {
		do_picking = true;
	} else if (cmd == 1) {
		do_picking = false;
	}
}

SceneNode *SceneNode::get_node(int i) const {
	return node_bank[i];
}

bool SceneNode::node_exist(int id) const
{
	return node_bank.find(id) != node_bank.end();
}