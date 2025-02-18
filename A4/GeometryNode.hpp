// Fall 2019

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
// #include "ray.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );

	Material *m_material;
	Primitive *m_primitive;

	virtual intersection intersect(const ray &ray);

	virtual intersection intersect(const ray &ray, std::list<glm::mat4> trans);
};
