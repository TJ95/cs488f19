// Fall 2019

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
	// Range of the Model Coordinate System
	const float neg = -1.0f;
	const float pos = 1.0f;

	// near/far planes, fov
	near = 5;
	far  = 20;
	fov  = 30;
	vp_w = 1.8;
	vp_h = 1.8;
	porting = false;

	// initialize vertices of the cube
	cube_verts.push_back(glm::vec4(neg, pos, neg, 1));
	cube_verts.push_back(glm::vec4(neg, pos, pos, 1));
	cube_verts.push_back(glm::vec4(pos, pos, neg, 1));
	cube_verts.push_back(glm::vec4(pos, pos, pos, 1)); // Top Vertices
	cube_verts.push_back(glm::vec4(neg, neg, neg, 1));
	cube_verts.push_back(glm::vec4(neg, neg, pos, 1));
	cube_verts.push_back(glm::vec4(pos, neg, neg, 1));
	cube_verts.push_back(glm::vec4(pos, neg, pos, 1)); // Bottom Vertices

	// initialize vertices of the initial ViewPort
	vp_load({0.9,0.9},{-0.9,-0.9});

	// define the edges of the cube in model coordinates
	initCube();
	initGnoms();
	imode = 0;

	// initialize M as identity matrix: Model -> World
	M_mat = glm::mat4(
    	1, 0, 0, 0,
    	0, 1, 0, 0,
    	0, 0, 1, 0,
    	0, 0, 0, 1
	);
	
	// create the camera view by performing the view transformation
	V_mat = glm::mat4(
		1, 0,  0, 0,
		0, 1,  0, 0,
		0, 0, -1, 0,
		0, 0,  8, 1
	);
	V_mat = inverse(V_mat);
	
	// create projection matrix
	P_mat = glm::mat4(
		1.f/tan(glm::radians(fov/2)),0,0,0,
		0, 1.f/tan(glm::radians(fov/2)),0,0,
		0,0, (far+near)/(far-near), 1,
		0, 0, (-2*far*near)/(far-near), 0
	);
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2019
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//---------------------------------------------------------------------------------------
void A2::vp_drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {
	auto temp0 = vec4{V0.x, V0.y, 0, 1};
	auto temp1 = vec4{V1.x, V1.y, 0, 1};
	temp0 = m_scale(vec3{vp_w/2,vp_h/2,1}) * temp0;
	temp0 = m_trans(vec3{offset_vp.x,offset_vp.y,0}) * temp0;
	
	temp1 = m_scale(vec3{vp_w/2,vp_h/2,1}) * temp1;
	temp1 = m_trans(vec3{offset_vp.x,offset_vp.y,0}) * temp1;
	
	drawLine({temp0.x,temp0.y},{temp1.x,temp1.y});
	//drawLine({temp0.x*0.9,temp0.y*0.9},{temp1.x*0.9,temp1.y*0.9});
}



//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	if (porting) {
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		if (vp_A.x < 0) vp_A.x = 0;
		if (vp_A.y < 0) vp_A.y = 0;
		if (vp_B.x < 0) vp_B.x = 0;
		if (vp_B.y < 0) vp_B.y = 0;
		if (vp_A.x > width) vp_A.x = width;
		if (vp_A.y > height) vp_A.y = height;
		if (vp_B.x > width) vp_B.x = width;
		if (vp_B.y > height) vp_B.y = height;
		auto tempA = vec2{2*vp_A.x/width-1,1-2*vp_A.y/height};
		auto tempB = vec2{2*vp_B.x/width-1,1-2*vp_B.y/height};
		vp_w = std::abs(tempA.x - tempB.x);
		vp_h = std::abs(tempA.y - tempB.y);
		offset_vp = vec2{(tempB.x+tempA.x)/2,(tempB.y+tempA.y)/2};
		vp_load(tempA, tempB);
		//drawLine(tempA, tempB);
	}

	// Draw Cube
	setLineColour(vec3(0.64f, 0.99f, 0.87f));
	for (int i = 0; i < 12; i++) {
		vec4 v1 = P_mat * V_mat * M_mat * cube_edges[i].first;
		vec4 v2 = P_mat * V_mat * M_mat * cube_edges[i].second;
		if (clipline(v1,v2)) {
			vp_drawLine({v1.x/v1.w,v1.y/v1.w},{v2.x/v2.w,v2.y/v2.w});
		}
	}

	// Draw Modelling Gnomon
	for (int i = 0; i < 3; i++) {
		auto temp = inverse(scale_record);
		vec4 v1 = P_mat * V_mat * M_mat * temp * gnom_m[i].first;
		vec4 v2 = P_mat * V_mat * M_mat * temp * gnom_m[i].second;
		switch (i) {
			case 0: setLineColour(vec3(1.0f,0.0f,0.0f));
					break;
			case 1: setLineColour(vec3(0.0f,1.0f,0.0f));
					break;
			case 2: setLineColour(vec3(0.0f,0.0f,1.0f));
					break;
		}
		if (clipline(v1,v2)) {
			vp_drawLine({v1.x/v1.w,v1.y/v1.w},{v2.x/v2.w,v2.y/v2.w});
		}
	}

	// Draw World Gnomon
	for (int i = 0; i < 3; i++) {
		vec4 v1 = P_mat * V_mat * gnom_w[i].first;
		vec4 v2 = P_mat * V_mat * gnom_w[i].second;
		switch (i) {
			case 0: setLineColour(vec3(0.0f,1.0f,1.0f));
					break;
			case 1: setLineColour(vec3(1.0f,1.0f,0.0f));
					break;
			case 2: setLineColour(vec3(1.0f,0.0f,1.0f));
					break;
		}
		if (clipline(v1,v2)) {
			vp_drawLine({v1.x/v1.w,v1.y/v1.w},{v2.x/v2.w,v2.y/v2.w});
		}
	}

	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	// drawLine({0.95,0.95},{-0.95,0.95});
	// drawLine({0.95,0.95},{0.95,-0.95});
	// drawLine({0.95,-0.95},{-0.95,-0.95});
	// drawLine({-0.95,0.95},{-0.95,-0.95});
	for (int i = 0; i < 4; i++) {
		drawLine(vp_verts[i],vp_verts[(i+1)%4]);
	}

	
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		// Add more gui elements here here ...
		ImGui::PushID( 0 ); // rotate V
		ImGui::PushID( 1 ); // translate V
		ImGui::PushID( 2 ); // persp
		ImGui::PushID( 3 ); // rotate M
		ImGui::PushID( 4 ); // translate M
		ImGui::PushID( 5 ); // scale M
		ImGui::PushID( 6 ); // viewport
		
		// rotate view
		ImGui::Text("Rotate View       (o)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 0);
		ImGui::PopID();

		// translate view
		ImGui::Text("Translate view    (e)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 1);
		ImGui::PopID();

		// perspective
		ImGui::Text("Perspective       (p)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 2);
		ImGui::PopID();

		// rotate Model
		ImGui::Text("Rotate Model      (r)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 3);
		ImGui::PopID();

		// translate Model
		ImGui::Text("Translate Model   (t)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 4);
		ImGui::PopID();

		// scale Model
		ImGui::Text("Scale Model       (s)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 5);
		ImGui::PopID();

		// Viewport
		ImGui::Text("Viewport          (v)");
		ImGui::SameLine();
		ImGui::RadioButton( "", &imode, 6);
		ImGui::PopID();


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Reset            (a)" ) ) {
			reset();
		}
		if( ImGui::Button( "Quit Application (q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}


		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
    	ImGui::Text("Near: %.1f, Far:%.1f", near,far);
		ImGui::Text("fov: %.1f°", fov);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	double diffx = xPos - prev_x;
	//double diffy = yPos - prev_y;
	double change_factor = diffx * 0.005;
	// rotate view
	if (imode == 0) {
		if (ImGui::IsMouseDown(0)) {
			//V_mat = V_mat*m_rotate(temp, vec3(0,change_factor,0));
			V_mat *= m_rotate(vec3(change_factor,0,0));
		}
		if (ImGui::IsMouseDown(1)) {
			V_mat *= m_rotate(vec3(0,0,change_factor));
		}
		if (ImGui::IsMouseDown(2)) {
			V_mat *= m_rotate(vec3(0,change_factor,0));
		}
	// translate view
	} else if (imode == 1){
		if (ImGui::IsMouseDown(0)) {
			V_mat *= m_trans(vec3(change_factor,0,0));
		}
		if (ImGui::IsMouseDown(1)) {
			V_mat *= m_trans(vec3(0,0,change_factor));
		}
		if (ImGui::IsMouseDown(2)) {
			V_mat *= m_trans(vec3(0,change_factor,0));
		}
	// Perspective
	} else if (imode == 2){
		if (ImGui::IsMouseDown(0)) {
			// if (change_factor > 0) {
			// 	fov += 8*change_factor;
			// } else {
			// 	fov += 5*change_factor;
			// }
			fov += 5*change_factor;
			fov = clamp((int)fov, 5, 160);
		}
		if (ImGui::IsMouseDown(1)) {
			far += 3*change_factor;
		}
		if (ImGui::IsMouseDown(2)) {
			near += 3*change_factor;
		}
		P_mat = glm::mat4(
		1/tan(glm::radians(fov/2)),0,0,0,
		0, 1/tan(glm::radians(fov/2)),0,0,
		0,0, (far+near)/(far-near), 1,
		0, 0, (-2*far*near)/(far-near), 0
		);
	// Rotate Model
	} else if (imode == 3) { 
		if (ImGui::IsMouseDown(0)) {
			M_mat = M_mat * m_rotate(vec3(change_factor,0,0));
		}
		if (ImGui::IsMouseDown(1)) {
			M_mat = M_mat * m_rotate(vec3(0,0,change_factor));
		}
		if (ImGui::IsMouseDown(2)) {
			M_mat = M_mat * m_rotate(vec3(0,change_factor,0));
		}
	// Translate Model
	} else if (imode == 4) {
		if (ImGui::IsMouseDown(0)) {
			M_mat = M_mat * m_trans(vec3(change_factor,0,0)); 
		}
		if (ImGui::IsMouseDown(1)) {
			M_mat = M_mat * m_trans(vec3(0,0,change_factor)); 
		}
		if (ImGui::IsMouseDown(2)) {
			M_mat = M_mat * m_trans(vec3(0,change_factor,0)); 
		}
	// Scale Model
	} else if (imode == 5) {
		if (ImGui::IsMouseDown(0)) {
			M_mat = M_mat * m_scale(vec3(1+change_factor, 1, 1)); 
			scale_record *= m_scale(vec3(1+change_factor, 1, 1)); 
		}
		if (ImGui::IsMouseDown(1)) {
			M_mat = M_mat * m_scale(vec3(1, 1, 1+change_factor)); 
			scale_record *= m_scale(vec3(1, 1, 1+change_factor)); 
		}
		if (ImGui::IsMouseDown(2)) {
			M_mat = M_mat * m_scale(vec3(1, 1+change_factor, 1)); 
			scale_record *= m_scale(vec3(1, 1+change_factor, 1)); 
		}

	// Viewport
	} else if (imode == 6) {
		if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (ImGui::IsMouseDown(0)) {
			if (!porting) {
				porting = true;
				vp_A.x = xPos;
				vp_A.y = yPos;
			} else {
				vp_B.x = xPos;
				vp_B.y = yPos;
			}
		} else {
			if (porting) {
				porting = false;
				vp_B.x = xPos;
				vp_B.y = yPos;
			}
		}
	}}

	prev_x = xPos;
	prev_y = yPos;
	//cout << "Current Pos:(" << xPos << "," << yPos << ")" << endl;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS) {
		if (key == 'O')
		{
			if (imode != 0) {
				cout << "Current Mode: Rotate View" << endl;
				imode = 0;
			}
			eventHandled = true;
		}
		if (key == 'E')
		{
			if (imode != 1) {
				cout << "Current Mode: Translate View" << endl;
				imode = 1;
			}
			eventHandled = true;
		}
		if (key == 'P')
		{
			if (imode != 2) {
				cout << "Current Mode: Perspective" << endl;
				imode = 2;
			}
			eventHandled = true;
		}
		if (key == 'R')
		{
			if (imode != 3) {
				cout << "Current Mode: Rotate Model" << endl;
				imode = 3;
			}
			eventHandled = true;
		}
		if (key == 'T')
		{
			if (imode != 4) {
				cout << "Current Mode: Translate Model" << endl;
				imode = 4;
			}
			eventHandled = true;
		}
		if (key == 'S')
		{
			if (imode != 5) {
				cout << "Current Mode: Scale Model" << endl;
				imode = 5;
			}
			eventHandled = true;
		}
		if (key == 'V')
		{
			if (imode != 6) {
				cout << "Current Mode: Viewport" << endl;
				imode = 6;
			}
			eventHandled = true;
		}
		if (key == 'Q')
		{
			cout << "Q key pressed, shutting down..." << endl;
			//quit application
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == 'A')
		{
			cout << "A key pressed, resetting.." << endl;
			reset();
			eventHandled = true;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Initialize the 12 edges of the cube
 */
void A2::initCube() {
	cube_edges.clear();
	// Top face
	cube_edges.push_back({cube_verts[0],cube_verts[1]});
	cube_edges.push_back({cube_verts[1],cube_verts[3]});
	cube_edges.push_back({cube_verts[3],cube_verts[2]});
	cube_edges.push_back({cube_verts[0],cube_verts[2]});

	// Bottom face
	cube_edges.push_back({cube_verts[4],cube_verts[5]});
	cube_edges.push_back({cube_verts[4],cube_verts[6]});
	cube_edges.push_back({cube_verts[5],cube_verts[7]});
	cube_edges.push_back({cube_verts[6],cube_verts[7]});
	
	// vertical edges
	cube_edges.push_back({cube_verts[0],cube_verts[4]});
	cube_edges.push_back({cube_verts[1],cube_verts[5]});
	cube_edges.push_back({cube_verts[2],cube_verts[6]});
	cube_edges.push_back({cube_verts[3],cube_verts[7]});
}

//----------------------------------------------------------------------------------------
/*
 * Initialize the axes for the reference models
 */
void A2::initGnoms() {
	vec4 og   = glm::vec4(0, 0, 0, 1);
	vec4 ax_y = glm::vec4(0, 1, 0, 1);
	vec4 ax_x = glm::vec4(1, 0, 0, 1);
	vec4 ax_z = glm::vec4(0, 0, 1, 1);

	gnom_w.clear();
	gnom_w.push_back({og, ax_x});
	gnom_w.push_back({og, ax_y});
	gnom_w.push_back({og, ax_z});
	
	gnom_m.clear();
	gnom_m.push_back({og, ax_x});
	gnom_m.push_back({og, ax_y});
	gnom_m.push_back({og, ax_z});
}

//----------------------------------------------------------------------------------------
/*
 * Compute the T-matrix for translation, rotation and Scaling
 * t stores the vec3 representing transformation in respect of the axis direction
 */
glm::mat4 A2::m_trans(const glm::vec3 &t) {
	auto transform = glm::mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		t.x, t.y, t.z, 1
	);
	return transform;
}

glm::mat4 A2::m_scale(const glm::vec3 &t) {
	auto transform = glm::mat4(
		t.x, 0,   0, 0,
		0, t.y,   0, 0,
		0,   0, t.z, 0,
		0,   0,   0, 1
	);
	return transform;
}

glm::mat4 A2::m_rotate(const glm::vec3 &t) {
	auto transform_x = glm::mat4(
		1,           0,          0, 0,
		0,    cos(t.x),   sin(t.x), 0,
		0,   -sin(t.x),   cos(t.x), 0,
		0,           0,          0, 1
	);

	auto transform_y = glm::mat4(
		cos(t.y), 0,-sin(t.y), 0,
		       0, 1,        0, 0,
		sin(t.y), 0, cos(t.y), 0,
		       0, 0,        0, 1
	);

	auto transform_z = glm::mat4(
		  cos(t.z), sin(t.z), 0, 0,
		 -sin(t.z), cos(t.z), 0, 0,
		         0,        0, 1, 0,
				 0,        0, 0, 1
	);

	return transform_x * transform_y * transform_z;
}

//----------------------------------------------------------------------------------------
/*
 * Reset everything to default
 */
void A2::reset(){
	// global variables
	imode = 0;
	prev_x = 0;
	prev_y = 0;
	fov = 30;
	far = 20;
	near = 5;
	porting = false;
	scale_record = mat4();
	initCube();
	initGnoms();
	offset_vp = {0,0};

	// viewport
	vp_load({0.9,0.9},{-0.9,-0.9});
	vp_w = 1.8;
	vp_h = 1.8;
	vp_A = vec2{0.9,0.9};
	vp_B = vec2{-0.9,-0.9};
	// M V P matrices
	M_mat = mat4();
	V_mat = glm::mat4(
		1, 0,  0, 0,
		0, 1,  0, 0,
		0, 0, -1, 0,
		0, 0,  8, 1
	);
	P_mat = glm::mat4(
		1/(glm::radians(fov/2)),0,0,0,
		0, 1/(glm::radians(fov/2)),0,0,
		0,0, (far+near)/(far-near), 1,
		0, 0, (-2*far*near)/(far-near), 0
	); 
}

//----------------------------------------------------------------------------------------
/*
 * Prepare the Viewport vector
 */
void A2::vp_load(const vec2 v0, const vec2 v1) {
	vp_verts.clear();
	//vp_w = abs(v1.x - v0.x);
	//vp_h = abs(v1.y - v0.y);
	if (v0.x < v1.x && v0.y < v1.y) { // B at top-right of A
		vp_verts.push_back(vec2{v1.x,v1.y});
		vp_verts.push_back(vec2{v1.x,v0.y});
		vp_verts.push_back(vec2{v0.x,v0.y});
		vp_verts.push_back(vec2{v0.x,v1.y});
	} else if (v0.x < v1.x && v0.y > v1.y) { // B at Bot-right of A
		vp_verts.push_back(vec2{v0.x,v0.y});
		vp_verts.push_back(vec2{v1.x,v0.y});
		vp_verts.push_back(vec2{v1.x,v1.y});
		vp_verts.push_back(vec2{v0.x,v1.y});
	} else if (v0.x > v1.x && v0.y < v1.y) { // B at top-left of A
		vp_verts.push_back(vec2{v0.x,v1.y});
		vp_verts.push_back(vec2{v0.x,v0.y});
		vp_verts.push_back(vec2{v1.x,v0.y});
		vp_verts.push_back(vec2{v1.x,v1.y});
	} else if (v0.x > v1.x && v0.y > v1.y) { // B at Bot-left of A
		vp_verts.push_back(vec2{v0.x,v0.y});
		vp_verts.push_back(vec2{v0.x,v1.y});
		vp_verts.push_back(vec2{v1.x,v1.y});
		vp_verts.push_back(vec2{v1.x,v0.y});
	}
}

//----------------------------------------------------------------------------------------
/*
 * Clipper
 */
float A2::cleft(const vec4 p) {
  return p.w + p.x;
}

float A2::cright(const vec4 p) {
  return p.w - p.x;
}

float A2::cbot(const vec4 p) {
  return p.w + p.y;
}

float A2::ctop(const vec4 p) {
  return p.w - p.y;
}

float A2::cnear(const vec4 p) {
  return p.w - p.z;
}

float A2::cfar(const vec4 p) {
  return p.w + p.z;
}

bool A2::clipline(vec4 &v1, vec4 &v2) {

  std::vector<std::function<float(const glm::vec4)> > clip_tests {
    cleft, cright, cbot, ctop, cnear, cfar
  };

  int i = 0;

  for (const auto test : clip_tests) {
    i += 1;
    float wecV1 = test(v1);
    float wecV2 = test(v2);

    if (wecV1 < 0 && wecV2 < 0) {
      return false;
    }

    if (wecV1 >= 0 && wecV2 >= 0) {
      continue;
    }

    float a = wecV1 / (wecV1 - wecV2);

    if (wecV1 < 0) {
      v1 = v1 + a * (v2 - v1);
    } else {
      v2 = v1 + a * (v2 - v1);
    }
  }

  return true;
}