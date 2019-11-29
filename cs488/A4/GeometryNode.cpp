// Fall 2019

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
	, m_texture( nullptr )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

intersection GeometryNode::intersect(const ray &ray) {
	auto origin = invtrans * ray.origin;
	auto dir = invtrans * ray.dir;
	class ray temp_ray(origin, dir);
	intersection intersec = m_primitive->intersect(ray);
	if (intersec.hit) {
		intersec.mat = (PhongMaterial *)m_material;
		intersec.node = this;
	}
	return intersec;
}

intersection GeometryNode::intersect(const ray & ray, std::list<glm::mat4> transformations) {
	auto origin = invtrans * ray.origin;
	auto dir	= invtrans * ray.dir;

	class ray new_ray(origin, dir);
	
	intersection i = m_primitive->intersect(new_ray);
	
	if (i.hit) {
		// get the materials as usual
		i.mat = (PhongMaterial *)m_material;
		i.node = this;
	}
	
	// Test with all remaning child, and return the closet intersection point.
	for (auto child : children) {
		intersection child_i = child->intersect(new_ray, transformations);
		
		if (child_i.hit) {
			if (!i.hit || child_i.t < i.t) {
				i = child_i;
			}
		}
	}
	
	if (i.hit) {
		auto normal = glm::dvec3(i.normal);
		auto invtrans3 = glm::dmat3(invtrans);
		
		i.normal = glm::dvec4(glm::transpose(invtrans3) * normal, 0);
		i.normal = glm::normalize(i.normal);
		
		i.received_ray.origin = trans * i.received_ray.origin;
		i.received_ray.dir = trans * i.received_ray.dir;
		
		i.from_mat = ray.mat;
	}
	return i;
}
