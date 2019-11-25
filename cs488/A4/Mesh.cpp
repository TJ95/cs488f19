// Fall 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>
#include <glm/gtx/intersect.hpp>
#include "polyroots.hpp"
using namespace std;

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( ("Assets/" + fname).c_str() );

  	if (ifs.fail()) {
		std::cerr << "Unable to open file for reading." << std::endl;
		exit(1);
  	}
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::dvec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	float Infinity = std::numeric_limits<float>::infinity();

  float minX = Infinity;
  float minY = Infinity;
  float minZ = Infinity;

  float maxX = -Infinity;
  float maxY = -Infinity;
  float maxZ = -Infinity;

  for (const glm::vec3 &v: m_vertices) {
    minX = std::min(minX, v.x);
    minY = std::min(minY, v.y);
    minZ = std::min(minZ, v.z);

    maxX = std::max(maxX, v.x);
    maxY = std::max(maxY, v.y);
    maxZ = std::max(maxZ, v.z);
  }

  const glm::vec3 min{minX, minY, minZ};
  const glm::vec3 max{maxX, maxY, maxZ};
#ifdef RENDER_BOUNDING_VOLUMES
  boundingVolume = new NonhierSphere((max + min)/2, glm::length(max - min)/2);
#endif
}

Mesh::~Mesh() {
#ifdef RENDER_BOUNDING_VOLUMES
  delete boundingVolume;
#endif
}


intersection Mesh::intersect(const ray& ray) {
#ifdef RENDER_BOUNDING_VOLUMES
  if(!boundingVolume->intersect(ray).hit) return intersection();
#endif
  double Infinity = std::numeric_limits<double>::infinity();
  double t = Infinity;

#ifdef RENDER_BOUNDING_VOLUMES
  return boundingVolume->intersect(ray);
#else
  intersection result(ray,0);
  dvec3 normal;

  int i = -1;

  for (const Triangle& tri : m_faces) {
    i += 1;
    const glm::dvec3 P0{m_vertices.at(tri.v1)};
    const glm::dvec3 P1{m_vertices.at(tri.v2)};
    const glm::dvec3 P2{m_vertices.at(tri.v3)};

    const glm::dvec3 X{P1 - P0};
    const glm::dvec3 Y{P2 - P0};
    const glm::dvec3 Z{glm::dvec3(ray.dir*(-1))};

    const double D = glm::determinant(glm::dmat3{X, Y, Z});

    if (std::fabs(D) < 0.0001) {
      continue;
    }

    const glm::dvec3 R = glm::dvec3(ray.origin) - P0;

    const double Dx = glm::determinant(glm::dmat3{R, Y, Z});
    const double Dy = glm::determinant(glm::dmat3{X, R, Z});
    const double Dz = glm::determinant(glm::dmat3{X, Y, R});

    glm::dvec3 S{Dx/D, Dy/D, Dz/D};

    if (
      !(S.x >= 0) ||
      !(S.y >= 0) ||
      !(S.z >= 0) ||
      !((1 - S.x - S.y) >= 0)
    ) {
      continue;
    }

    if (S.z < t) {
      t = S.z;
	  normal = glm::normalize(glm::cross(X,Y));
    }
  }

  if (t == Infinity) {
    result.hit = false;

  } else {
	  result.hit = true;
	  result.t = t;
		// update normals accordingly
	  auto int_point = ray.origin + ray.dir * result.t;
    result.normal = dvec4(normal,0);
    result.primitive_int_point = int_point;	  
  }

  return result;
#endif
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
