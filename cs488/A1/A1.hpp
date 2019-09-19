// Fall 2019

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

	// additions
	virtual void dig();
	virtual void wall_edit(int i);
	virtual void teleport(float x, float y, float z);
	virtual void translate_avatar(int d);
	virtual void reset();

	private : void initGrid();

	// additions
	void initFloor();
	void initAvatar(float x, float y, float z);
	void initCube();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to floor geometry.
	GLuint m_floor_vao; // Vertex Array Object
	GLuint m_floor_vbo; // Vertex Buffer Object
	GLuint m_floor_ebo;
	glm::vec3 m_floor_color;

	// Fields related to cube geometry.
	GLuint m_cube_vao;
	GLuint m_cube_vbo;
	GLuint m_cube_ebo;
	glm::vec3 m_cube_color;

	// Fields related to avatar geometry.
	GLuint m_avatar_vao; // Vertex Array Object
	GLuint m_avatar_vbo; // Vertex Buffer Object
	GLuint m_avatar_ebo;
	glm::vec3 m_avatar_color;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	//stuff
	float colour[3];
	int current_col;
	float element_colors[3][3];
	int floor_color_index = 0;
	int cube_color_index = 1;
    int avatar_color_index = 2;

	// World rotation angle in radians.
    float m_world_rotation;
 
    // World scaling factor
    float m_world_size;

	// avatar
	float ava[24];
	// array that contains the vertex order for faces of a cube
	unsigned int inds[36] = {
		0, 1, 3, //
		1, 2, 3, // Bottom face
		1, 2, 5,
		6, 2, 5, // Front
		2, 3, 6,
		3, 6, 7, // Right
		0, 1, 5,
		0, 4, 5, // Left
		3, 4, 7,
		0, 3, 4, // Back
		4, 5, 6,
		4, 6, 7 // Top
	};
};
