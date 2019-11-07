// Fall 2019

#pragma once

#include "SceneNode.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include "cs488-framework/GlErrorCheck.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
protected:
	// Rendering the mesh
	virtual void render(
		const ShaderProgram &shader,
		const glm::mat4 &viewMatrix,
		BatchInfoMap &bim,
		std::deque<glm::mat4> &stack) const;

	virtual void updateShaderUniforms(
		const ShaderProgram &shader,
		const glm::mat4 &viewMatrix,
		std::deque<glm::mat4> &stack) const;
};
