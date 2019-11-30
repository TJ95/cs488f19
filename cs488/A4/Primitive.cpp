// Fall 2019

#include "Primitive.hpp"
#include "polyroots.hpp"
#include "Mesh.hpp"
#include <vector>

Primitive::~Primitive()
{
}

intersection Primitive::intersect(const ray &ray)
{
    intersection result(ray, 0);
    return result;
}

intersection Primitive::intersect(const ray & ray, std::list<glm::mat4> trans)
{
	auto result = m_primitive->intersect(ray, trans);
	return result;
}

Sphere::Sphere() {
    m_primitive = new NonhierSphere({0, 0, 0}, 1.0);
}

Sphere::~Sphere()
{
    delete m_primitive;
}

intersection Sphere::intersect(const ray & ray)
{
	intersection result = m_primitive->intersect(ray);
	return result;
}

Cube::Cube()
{
    m_primitive = new NonhierBox({0, 0, 0}, 1.0);
}

Cube::~Cube() {
    delete m_primitive;
}

intersection Cube::intersect(const ray &ray) {
    intersection result = m_primitive->intersect(ray);
    return result;
}

NonhierSphere::~NonhierSphere(){
}

intersection NonhierSphere::intersect(const ray & ray) {
    //cout << "hit-testing spheres.." << endl;
    // loading sphere info
    intersection result(ray, 0);
    glm::dvec4 center(m_pos.x, m_pos.y, m_pos.z,1);
    const glm::dvec4 &dir_ray = ray.dir;
    const glm::dvec4 &origin_ray = ray.origin;
    const glm::dvec4 &local_from = origin_ray - center;

    // solve quadratic formula for intersection points (r = (-b+-4ac)/2a)
    double a = glm::dot(dir_ray, dir_ray); // (dir^2, quadratic coef)
    double b = 2*glm::dot(dir_ray, local_from); // (2*dir*local, linear coeff)
    double c = glm::dot(local_from, local_from) - (m_radius * m_radius); // (local^2-radius^2, scalar coeff)
    double solns[2];
    // provided in polyroots
    size_t root_count = quadraticRoots(a,b,c,solns);
    // determine intersection points given # of roots
    if (root_count == 0) { // no roots, no intersection
        result.hit = false;
    } else if (root_count == 1) { // 1 root, hits sphere (tangent)
        if (solns[0] > 0) {
            result.t = solns[0];
            result.hit = true;
        } else {
            result.hit = false;
        }
    } else if (root_count == 2) {
        // if (true_root >= 0) {
        //     result.t = true_root;
        //     result.hit = true;
        // } else {
        //     result.hit = false;
        // }
        if (solns[0]>0&&solns[1]>0) {
            result.t = std::min(solns[0],solns[1]);
        } else {
            result.t = solns[0] > 0 ? solns[0] : solns[1]; 
        }
        result.hit = result.t > 0;
    }
    // update normals accordingly
    auto int_point = ray.origin + ray.dir * result.t;
    glm::dvec4 normal = glm::normalize(int_point - center);
    result.normal = normal;
    result.primitive_int_point = int_point;
    //if (result.hit) cout << "hit!!" << endl;
    return result;
}

intersection NonhierSphere::intersect(const ray & ray, std::list<glm::mat4> transformations) {
    return NonhierSphere::intersect(ray);
}

NonhierBox::~NonhierBox()
{
}

// intersection NonhierBox::intersect(const ray &ray) {
//     intersection result(ray, 0);
//     glm::vec3 x{m_size, 0, 0};
//     glm::vec3 y{0, m_size, 0};
//     glm::vec3 z{0, 0, m_size};
//     glm::vec3 m_pos_end = m_pos + x + y + z;
//     m_pos_2 = m_pos_end;
//     glm::vec3 normal;
//     glm::vec3 points[18] = {
//         m_pos, x, y,
//         m_pos, x, z,
//         m_pos, y, z,
//         m_pos_end, -x, -y,
//         m_pos_end, -x, -z,
//         m_pos_end, -y, -z
//     };

//     // glm::vec3 A{ray.origin};
//     // glm::vec3 B{ray.dir*-1};
//     auto to = ray.origin + ray.dir;
//     //to*= -1;
//     glm::vec3 A{ray.origin};
//     glm::vec3 B(to);

//     double Infinity = std::numeric_limits<double>::infinity();
//     double t = Infinity;

//     for (unsigned int i = 0; i < 6; i += 1) {
//         glm::vec3 P = points[i * 3];
//         glm::vec3 P1mP = points[i * 3 + 1];
//         glm::vec3 P2mP = points[i * 3 + 2];
//         glm::mat3 M{
//         P1mP,
//         P2mP,
//         A - B
//         };

//         double determinant = glm::determinant(M);
//         if (std::fabs(determinant) < 0.0001) {
//             continue;
//         }

//         glm::vec3 R = A - P;
//         glm::vec3 S = glm::inverse(M) * R;

//         if (
//             !(0.0 <= S.x && S.x <= 1.0) ||
//             !(0.0 <= S.y && S.y <= 1.0)
//            ) {
//             continue;
//         }

//         if (S.z < t && S.z > 0) {
//             t = S.z;
//             normal = glm::normalize(glm::cross(P1mP,P2mP));
//         }
//     }

//     if (t == Infinity) {
//         result.hit = false;
//     } else {
//         result.t = t;
//         result.hit = true;
//     }
//     // update normals accordingly
//     auto int_point = ray.origin + ray.dir * result.t;
//     result.normal = vec4(normal,0);
//     result.primitive_int_point = int_point;
//     return result;
// }

intersection NonhierBox::intersect(const ray &ray){
    std::vector<glm::vec3> vertices = {
        m_pos + glm::vec3(0, 0, m_size),
        m_pos + glm::vec3(m_size, 0, m_size),
        m_pos + glm::vec3(m_size, m_size, m_size),
        m_pos + glm::vec3(0, m_size, m_size),

        m_pos + glm::vec3(0, 0, 0),
        m_pos + glm::vec3(m_size, 0, 0),
        m_pos + glm::vec3(m_size, m_size, 0),
        m_pos + glm::vec3(0, m_size, 0)};

    std::vector<Triangle> faces = {
        Triangle(2, 3, 0),
        Triangle(0, 1, 2),
        Triangle(2, 1, 5),
        Triangle(5, 6, 2),
        Triangle(3, 7, 4),
        Triangle(4, 0, 3),
        Triangle(7, 6, 5),
        Triangle(5, 4, 7),

        Triangle(1, 0, 4),
        Triangle(4, 5, 1),
        Triangle(2, 6, 7),
        Triangle(7, 3, 2)};

    double Infinity = std::numeric_limits<double>::infinity();
    double t = Infinity;

    intersection result(ray,0);
    dvec3 normal;

    int i = -1;

    for (const Triangle &tri : faces)
    {
        i += 1;
        const glm::dvec3 P0{vertices.at(tri.v1)};
        const glm::dvec3 P1{vertices.at(tri.v2)};
        const glm::dvec3 P2{vertices.at(tri.v3)};

        const glm::dvec3 X{P1 - P0};
        const glm::dvec3 Y{P2 - P0};
        const glm::dvec3 Z{glm::dvec3(ray.dir * (-1))};

        const double D = glm::determinant(glm::dmat3{X, Y, Z});

        if (std::fabs(D) < 0.0001)
        {
            continue;
        }

        const glm::dvec3 R = glm::dvec3(ray.origin) - P0;

        const double Dx = glm::determinant(glm::dmat3{R, Y, Z});
        const double Dy = glm::determinant(glm::dmat3{X, R, Z});
        const double Dz = glm::determinant(glm::dmat3{X, Y, R});

        glm::dvec3 S{Dx / D, Dy / D, Dz / D};

        if (
            !(S.x >= 0) ||
            !(S.y >= 0) ||
            !(S.z >= 0) ||
            !((1 - S.x - S.y) >= 0))
        {
            continue;
        }

        if (S.z < t)
        {
            t = S.z;
            normal = glm::normalize(glm::cross(X, Y));
        }
    }

    if (t == Infinity)
    {
        result.hit = false;
    }
    else
    {
        result.hit = true;
        result.t = t;
        // update normals accordingly
        auto int_point = ray.origin + ray.dir * result.t;
        result.normal = dvec4(normal, 0);
        result.primitive_int_point = int_point;
    }

    if (result.hit) {
        result.primitive_int_point = result.received_ray.origin + result.received_ray.dir * result.t;
    }

    return result;
}

intersection Plane::intersect(const ray &ray)
{
    double t = - ray.origin.y / ray.dir.y;
    auto hitPoint = ray.origin + ray.dir * t;
    
    intersection intersect(ray, 0);
    
    if (hitPoint.x <= 1 && hitPoint.x >= -1 && hitPoint.z <= 1 && hitPoint.z >= -1 && t > 0) {
        intersect.t = t;
        intersect.hit = true;
        if (ray.origin.y < 0) {
            intersect.normal = glm::dvec4(0, -1, 0, 0);
        } else {
            intersect.normal = glm::dvec4(0, 1, 0, 0);
        }
        intersect.primitive_int_point = hitPoint;
    } else {
        intersect.hit = false;
    }
    
    return intersect;
}
