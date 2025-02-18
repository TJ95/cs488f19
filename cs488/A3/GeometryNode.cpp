// Fall 2019

#include "GeometryNode.hpp"
#include "SceneNode.hpp"

using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

//---------------------------------------------------------------------------------------
// Rendering the mesh
void GeometryNode::render(
	const ShaderProgram &shader,
	const mat4 &viewMatrix,
	BatchInfoMap &bim,
	std::deque<mat4> &stack) const 
{
	// perform current transformation
	stack.push_back(trans);
	// recursive calls for the children
	for (const SceneNode *node : this->children){
		node->render(shader, viewMatrix, bim, stack);
	}
	// current obj render
	updateShaderUniforms(shader, viewMatrix, stack);
	BatchInfo batchInfo = bim[meshId];
	shader.enable();
	glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	shader.disable();
	// remove executed transformation
	stack.pop_back();
}

//---------------------------------------------------------------------------------------
void GeometryNode::updateShaderUniforms(
	const ShaderProgram &shader,
	const mat4 &viewMatrix,
	std::deque<mat4> &stack) const
{
	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView;
		for (auto itor = stack.rbegin(); itor < stack.rend(); itor++){
			modelView = (*itor) * modelView;
		}
		modelView = viewMatrix * modelView;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if (do_picking) {
			float r = float(m_nodeId & 0xff) / 255.0f;
			float g = float((m_nodeId >> 8) & 0xff) / 255.0f;
			float b = float((m_nodeId >> 16) & 0xff) / 255.0f;

			location = shader.getUniformLocation("material.kd");
			glUniform3f(location, r, g, b);
			CHECK_GL_ERRORS;
		} else {
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;

			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd;
			if (!isSelected)
			{
				kd = this->material.kd;
			} else {
				kd = {1, 1, 0};
			}
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
		}
	}
	shader.disable();
}
