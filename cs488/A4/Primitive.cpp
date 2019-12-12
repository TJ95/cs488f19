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
    //cout << "haha" << endl;
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
    result.int_prim_type = P_SPHERE;
    result.sradius = m_radius;
    result.spos = (dvec3)m_pos;
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
        //result.int_prim_type = P_CUBE;
    }

    if (result.hit) {
        result.primitive_int_point = result.received_ray.origin + result.received_ray.dir * result.t;
        result.int_prim_type = P_CUBE;
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
        intersect.int_prim_type = P_PLANE;
    } else {
        intersect.hit = false;
    }
    
    return intersect;
}

Cone::Cone()
{
    
}

Cone::~Cone()
{
    
}

intersection Cone::intersect(const ray &ray)
{
    // x^2+z^2-y^2=0
    // r = o + st
    // solving quadratic function: x1, x2 = (1/2a)*(-b(+/-)4*a*c)
    intersection result(ray, 0);
	double a = ray.dir.x * ray.dir.x + ray.dir.z * ray.dir.z - ray.dir.y * ray.dir.y;
	double b = 2 * (ray.origin.x * ray.dir.x + ray.origin.z * ray.dir.z - ray.origin.y * ray.dir.y);
	double c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - ray.origin.y * ray.origin.y;
	
	double roots[2];
	size_t num_roots = quadraticRoots(a, b, c, roots);
	
	if (num_roots == 0) {
		result.hit = false;
		
	} else if (num_roots == 1) {
		double t = roots[0];
		double y = ray.origin.y + ray.dir.y * t;
		if (y <= 1 && y >= 0) {
			result.t = roots[0];
			result.hit = true;
			
			// cross x&z to get the normal of the tangent surface
			glm::dvec4 hitpoint = ray.origin + ray.dir * t;
			double x2_z2_sqrt = sqrt( hitpoint.x * hitpoint.x + hitpoint.z * hitpoint.z );
			glm::dvec3 p_x {1.0, hitpoint.x / x2_z2_sqrt, 0.0};
			glm::dvec3 p_z {0.0, hitpoint.z / x2_z2_sqrt, 1.0};
			glm::dvec3 normal = glm::cross(p_x, p_z);
			result.normal = glm::dvec4(glm::normalize(normal), 0);
			result.primitive_int_point = hitpoint;
		} else {
			result.hit = false;
		}
	// 2 roots, light ray passes through the polygon
	} else if (num_roots == 2) {
		double t1 = roots[0];
		double t2 = roots[1];
		
		glm::dvec4 p1 = ray.origin + ray.dir * t1;
		glm::dvec4 p2 = ray.origin + ray.dir * t2;
		
		if (t1 > t2) {
			// p2 will be the close hit point
			std::swap(t1, t2);
			std::swap(p1, p2);
		}
		
		if (p1.y < 0) {
			if (p2.y < 0) {
				// Missed
				result.hit = false;
			} else {
				// if p2.y is above 0, we need to see if p2 is a valid hitpoint or not.
				if (p2.y <= 1 && p2.y >= 0) {
					if (t2 > 0) {
						result.hit = true;
						result.t = t2;
						
						glm::dvec4 hitpoint = ray.origin + ray.dir * t2;
						double x2_z2_sqrt = sqrt( hitpoint.x * hitpoint.x + hitpoint.z * hitpoint.z );
						glm::dvec3 p_x {1.0, hitpoint.x / x2_z2_sqrt, 0.0};
						glm::dvec3 p_z {0.0, hitpoint.z / x2_z2_sqrt, 1.0};
						glm::dvec3 normal = glm::cross(p_x, p_z);
						result.normal = glm::dvec4(glm::normalize(normal), 0);
                        result.primitive_int_point = ray.origin + ray.dir * result.t;
					} else {
						result.hit = false;
					}
				} else {
					result.hit = false;
				}
				
			}
			
		} else if (p1.y >= 0 && p1.y <= 1) {
			// Hit the cone.
			if (t1 <= 0) {
				result.hit = false;
			} else {
				result.hit = true;
				result.t = t1;
				
				auto hitpoint = ray.origin + ray.dir * t1;
				double x2_z2_sqrt = sqrt( hitpoint.x * hitpoint.x + hitpoint.z * hitpoint.z );
				glm::dvec3 p_x {1.0, hitpoint.x / x2_z2_sqrt, 0.0};
				glm::dvec3 p_z {0.0, hitpoint.z / x2_z2_sqrt, 1.0};
				glm::dvec3 normal = glm::cross(p_x, p_z);
				result.normal = glm::dvec4(glm::normalize(normal), 0);
                result.primitive_int_point = hitpoint;
			}
			
		} else if (p1.y > 1) {
			if (p2.y > 1) {
				result.hit = false;
			} else {
				if (p2.y <= 1 && p2.y >= 0) {
					if (t2 > 0) {
						// Hit the top cap
						double th = ( 1 - ray.origin.y ) / ray.dir.y;
						auto hitpoint = ray.origin + ray.dir * th;
						
						if (th >= 0 && (hitpoint.x <= 1 && hitpoint.x >= -1) && (hitpoint.z <= 1 && hitpoint.z >= -1)) {
							result.hit = true;
							result.t = th;
							result.normal = glm::dvec4(0, 1, 0, 0);
                            result.primitive_int_point = hitpoint;
						} else {
							result.hit = false;
						}
						
					} else {
						result.hit = false;
					}
					
				} else {
					result.hit = false;
				}
				
			}
		}	
	}
    if (result.hit) {
        double x = result.received_ray.origin.x;
        double z = result.received_ray.origin.z;
        double y = result.received_ray.origin.y;
        if (x * x + z * z <= y * y ) {
            result.normal = -result.normal;
        }
    }
    return result;
}

Cylinder::Cylinder()
{
	
}

Cylinder::~Cylinder()
{
	
}


intersection Cylinder::intersect(const ray &ray)
{
    intersection result(ray, 0);
    double a = ray.dir.x * ray.dir.x + ray.dir.z * ray.dir.z;
    double b = 2 * ray.origin.x * ray.dir.x + 2 * ray.origin.z * ray.dir.z;
    double c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - 1;
    
    double roots[2];
    size_t num_roots = quadraticRoots(a, b, c, roots);

    if (num_roots == 0) {
        // no root
        result.hit = false;
        
    } else if (num_roots == 1) {
        double t = roots[0];
        double y = ray.origin.y + ray.dir.y * t;
        if (y <= 1 && y >= -1) {
            result.t = roots[0];
            result.hit = true;
			
			auto hitpoint = ray.origin + ray.dir * t;
			result.normal = glm::normalize(glm::dvec4(hitpoint.x, 0, hitpoint.z, 0));
            result.primitive_int_point = hitpoint;
        } else {
            result.hit = false;
        }
        
    } else if (num_roots == 2) {
        double t1 = roots[0];
        double t2 = roots[1];
        
        double y1 = ray.origin.y + ray.dir.y * t1;
        double y2 = ray.origin.y + ray.dir.y * t2;
        
        if (t1 > t2) {
            std::swap(t1, t2);
            std::swap(y1, y2);
        }
		
		if (y1 < -1) {
			
			if (y2 < -1) {
				result.hit = false;
			} else {
				float th = t1 + (t2 - t1) * (y1 + 1) / (y1 - y2);
				if (th <= 0)
					result.hit = false;
				else
					result.hit = true;
				result.t = th;
				result.normal = glm::dvec4(0, -1, 0, 0);
                result.primitive_int_point = ray.origin + ray.dir * (double)th;
			}
			
		} else if (y1 >= -1 && y1 <= 1) {
			// Hit the cylinder bit.
			if (t1 <= 0) {
				result.hit = false;
			} else {
				result.hit = true;
				result.t = t1;
				
				auto hitpoint = ray.origin + ray.dir * t1;
				result.normal = glm::normalize(glm::dvec4(hitpoint.x, 0, hitpoint.z, 0));
                result.primitive_int_point = hitpoint;
			}
			
		} else if (y1 > 1) {
			if (y2 > 1) {
				result.hit = false;
			} else {
				// hit the top cap
				float th = t1 + (t2 - t1) * (y1 - 1) / (y1 - y2);
				if (th <= 0)
					result.hit = false;
				else
					result.hit = true;
				
				result.t = th;
				result.normal = glm::dvec4(0, 1, 0, 0);
                result.primitive_int_point = ray.origin + ray.dir * (double)th;
			}
		}
	}
    
    if (result.hit) {
        // inside hit test
        double x = result.received_ray.origin.x;
        double z = result.received_ray.origin.z;
        double y = result.received_ray.origin.y;
        
        if (std::abs(y) <= 1 && (x * x + z * z <= 1)) {
            result.normal = -result.normal;
        }
    }
    
    return result;
}

intersection NonhierTorus::intersect(const ray& ray) {
  //cout << " here " << endl;
  intersection result(ray, 0);
  // http://www.emeyex.com/site/projects/raytorus.pdf
  dvec3 p = (dvec3)ray.origin - m_pos;
  dvec3 d = (dvec3)ray.dir;

  double R2 = m_oradius*m_oradius;
  double r2 = m_iradius*m_iradius;
  double a = glm::dot(d,d);
  double b = 2 * glm::dot(p,d);
  double y = glm::dot(p,p) - r2 - R2;

  double _A = 1.0 / (a*a);
  double B = 2*a*b;
  double C = (b*b) + 2*a*y + 4*R2*(d[2]*d[2]);
  double D = 2*b*y + 8*R2*p[2]*d[2];
  double E = (y*y) + 4*R2*(p[2]*p[2]) - 4*R2*r2;

  double roots[4];

  // solving for t
  size_t num_roots = quarticRoots(B * _A, C * _A, D * _A, E * _A, roots);

  // If there are no roots, then no intersection
  if(num_roots == 0) {
      result.hit = false;
      return result;
  }

  double t = std::numeric_limits<double>::infinity(); 
  for(size_t i = 0; i < num_roots; i++)
  {
    t = (roots[i] > 0 && roots[i] < t) ? roots[i] : t;
  }

  if(std::isinf(t)) {
      result.hit = false;
      return result;
  }

  // To find the surface normal, we take the partial derivative of the implicit formula of the torus
  // with respect to each of the coordinates and then plug in the coordinate values from the intersection point
  dvec4 Q = ray.origin + t*ray.dir;
  double qx2 = Q[0]*Q[0];
  double qy2 = Q[1]*Q[1];
  double qz2 = Q[2]*Q[2];
  double nx = 4*Q[0]*(qx2 + qy2 + qz2 - r2 - R2);
  double ny = 4*Q[1]*(qx2 + qy2 + qz2 - r2 - R2);
  double nz = 4*Q[2]*(qz2 + qy2 + qz2 - r2 - R2) + 8*R2*Q[2];

//   double theta = asin((Q[2]-m_pos[2]) / m_iradius);
//   double phi = asin((Q[1]-m_pos[1]) / (m_oradius + m_iradius*cos(theta)));

//   j.u = 0.5 + phi / M_PI;
//   j.v = 0.5 + theta / M_PI;
  result.hit = true;
  result.t = t;
  result.normal = dvec4(nx, ny, nz, 0);
  result.primitive_int_point = Q;
  //cout << Q << endl;
  return result;
  
}

NonhierTorus::~NonhierTorus() {}

Torus::Torus() {
    m_primitive = new NonhierTorus({0,0,0},1.0,0.1);
}

Torus::~Torus()
{
    delete m_primitive;
}

intersection Torus::intersect(const ray& ray)  {
    //cout << "here" << endl;
  intersection result =  m_primitive->intersect(ray);
  return result;
}
