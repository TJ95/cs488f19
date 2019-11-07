// Fall 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};

class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);
	//additions
	void vp_drawLine(const glm::vec2 &V0, const glm::vec2 &V1);
	void initCube();
	void initGnoms();
	bool Clipper(glm::vec4 &p1, glm::vec4 &p2);
	// compute the t-matrices for the 3 types of transformations
	glm::mat4 m_trans(const glm::vec3 &t);
	glm::mat4 m_rotate(const glm::vec3 &t);
	glm::mat4 m_scale(const glm::vec3 &t);
	void reset();
	void vp_load(glm::vec2 v0, glm::vec2 v1);


	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	// additions
	std::vector<glm::vec4> cube_verts;
	std::vector<std::pair<glm::vec4, glm::vec4>> cube_edges;
	std::vector<std::pair<glm::vec4, glm::vec4>> gnom_w;
	std::vector<std::pair<glm::vec4, glm::vec4>> gnom_m;

	float near;
	float far;
	float fov;
	float prev_x;
	float prev_y;
	glm::mat4 M_mat;
	glm::mat4 V_mat;
	glm::mat4 P_mat;
	glm::mat4 scale_record = glm::mat4();
	// 0-6 correspond to the 7 interaction modes
	int imode;
	// viewport (4 corner vertices)
	std::vector<glm::vec2> vp_verts;
	glm::vec2 vp_A;
	glm::vec2 vp_B;
	glm::vec2 offset_vp;
	float vp_w;
	float vp_h;
	float vp_w_old;
	float vp_h_old;
	bool porting = false;

	// Clippers
	static float cleft(const glm::vec4 p);
	static float cright(const glm::vec4 p);
	static float cbot(const glm::vec4 p);
	static float ctop(const glm::vec4 p);
	static float cnear(const glm::vec4 p);
	static float cfar(const glm::vec4 p);
	bool clipline(glm::vec4 &v1, glm::vec4 &v2);
	
};
