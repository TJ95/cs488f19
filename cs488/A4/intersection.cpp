#include "intersection.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
//#include "ray.hpp"

// ctor 1
intersection::intersection(const ray &ray, double t, bool hit) : received_ray(ray), t(t), hit(hit) {}

// ctor 2
intersection::intersection(const ray &ray, double t) : received_ray(ray), t(t), hit(false) {}

// ctor 3
intersection::intersection() : received_ray(ray(glm::vec4(), glm::vec4())), t(0), hit(false) {}

Texture * intersection::textureInfo() const {
    if (node->m_nodeType == NodeType::GeometryNode) {
        auto temp = dynamic_cast<GeometryNode *>(node);
        return temp->m_texture;
    } else {
        return nullptr;
    }
}