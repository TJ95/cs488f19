// Fall 2019

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.55, 0.55, 0.55, 1.0);

	// set booleans
	do_picking = false;
	display_circle = false;
	z_buffer_active = true;
	bf_cull = false;
	ff_cull = false;
	imode = 0;

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(10.0f, 10.0f, 10.0f);
	m_light.rgbIntensity = vec3(0.6f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i(location, do_picking ? 1 : 0);

		//-- Set LightSource uniform for the scene:
		if (!do_picking) {
			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.05f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_MenuBar);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Application"))
			{
				if (ImGui::MenuItem("Reset Position", "I"))
				{
					resetPosition();
				}
				if (ImGui::MenuItem("Reset Orientation", "O"))
				{
					resetOrientation();
				}
				if (ImGui::MenuItem("Reset Joints", "S"))
				{
					//resetJoint();
				}
				if (ImGui::MenuItem("Reset All", "A"))
				{
					resetAll();
				}
				if (ImGui::MenuItem("Quit", "Q"))
				{
					glfwSetWindowShouldClose(m_window, GL_TRUE);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edits"))
			{
				if (ImGui::MenuItem("Undo", "U"))
				{
				}
				if (ImGui::MenuItem("Redo", "R"))
				{
				}
				
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Options"))
			{
				ImGui::Checkbox("Circle", &display_circle);
				ImGui::Checkbox("Z Buffer", &z_buffer_active);
				ImGui::Checkbox("Backface Culling", &bf_cull);
				ImGui::Checkbox("Frontface Culling", &ff_cull);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::PushID(0);
		ImGui::PushID(1);

		// Position/orientation
		ImGui::Text("Position/Orientation      (P)");
		ImGui::SameLine();
		ImGui::RadioButton("", &imode, 0);
		ImGui::PopID();
		// Joint
		ImGui::Text("Joints Mode               (J)");
		ImGui::SameLine();
		ImGui::RadioButton("", &imode, 1);
		ImGui::PopID();

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;

		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (z_buffer_active) {
		glEnable(GL_DEPTH_TEST);
	}
	
	culling();

	renderSceneGraph(*m_rootNode);

	glDisable(GL_CULL_FACE);

	if (z_buffer_active) {
		glDisable(GL_DEPTH_TEST);
	}

	if (display_circle){
			renderArcCircle();
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	// retrive transformation from root node
	deque<glm::mat4> t_stack;
	glm::mat4 prev_state = root.get_transform();
	// apply transformations
	m_rootNode->trans = m_rootNode->trans * rotation;
	m_rootNode->trans = translation * m_rootNode->trans;
	// render the mesh recursively
	m_rootNode->render(m_shader, m_view, m_batchInfoMap, t_stack);
	m_rootNode->set_transform(prev_state);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle()
{
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
	GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
	float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
	glm::mat4 M;
	if( aspect > 1.0 ) {
		M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
	} else {
		M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
	}
	glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
	glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	double diff_x = xPos - prev_x;
	double diff_y = yPos - prev_y;
	//double diffy = yPos - prev_y;
	if (imode == 0) {
		if (ImGui::IsMouseDown(0)){
			translation = translate(translation, vec3(diff_x*0.01, -diff_y*0.01, 0));
		}
		if (ImGui::IsMouseDown(2)){
			translation = translate(translation, vec3(0, 0, diff_y*0.01));
		}
		if (ImGui::IsMouseDown(1)){
			if (prev_x != xPos || prev_y != yPos) {
				glm::vec3 va = get_arcball_vector(prev_x, prev_y);
				glm::vec3 vb = get_arcball_vector(xPos, yPos);

				float angle = acos(std::min(1.0f, glm::dot(va, vb)))*0.05;

				glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
				glm::vec4 axis_view = {axis_in_camera_coord.x,
									   axis_in_camera_coord.y,
									   axis_in_camera_coord.z, 0}; // 4-vector
				glm::vec4 axis_world = inverse(m_view) * axis_view;
				rotation = glm::rotate(rotation, glm::degrees(angle), {axis_world.x, axis_world.y, axis_world.z});
			}
		}
	} else if (imode == 1) { // joint mode
		if (ImGui::IsMouseDown(2)) {
			float angle = diff_x*2;

			for (auto joint : active_nodes) {
				//cout << joint->m_nodeId << endl;
				joint->rotate('x',angle);
			}
		}
	}

	prev_x = xPos;
	prev_y = yPos;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (imode == 1) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(m_window, &xpos, &ypos);
			do_picking = true;
			m_rootNode->picking_switch(0);

			uploadCommonSceneUniforms();
			glClearColor(1.0, 1.0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.55, 0.55, 0.55, 1.0);

			draw();

			// I don't know if these are really necessary anymore.
			// glFlush();
			// glFinish();

			CHECK_GL_ERRORS;

			// Ugly -- FB coordinates might be different than Window coordinates
			// (e.g., on a retina display).  Must compensate.
			xpos *= double(m_framebufferWidth) / double(m_windowWidth);
			// WTF, don't know why I have to measure y relative to the bottom of
			// the window in this case.
			ypos = m_windowHeight - ypos;
			ypos *= double(m_framebufferHeight) / double(m_windowHeight);

			GLubyte buffer[4] = {0, 0, 0, 0};
			// A bit ugly -- don't want to swap the just-drawn false colours
			// to the screen, so read from the back buffer.
			glReadBuffer(GL_BACK);
			// Actually read the pixel at the mouse location.
			glReadPixels(int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			CHECK_GL_ERRORS;

			// Reassemble the object ID.
			unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
			//cout << what << endl;
			if (m_rootNode->node_exist(what)) {
				auto * node = m_rootNode->get_node(what);
				node->isSelected = !node->isSelected;
				if (what == 20) {
						m_rootNode->get_node(21)->isSelected = node->isSelected;
				}
				if (what == 21) {
						m_rootNode->get_node(20)->isSelected = node->isSelected;
				}
				if (what == 1) {
					node->isSelected = false;
				}
				if (node->parent->m_nodeType == NodeType::JointNode) {
					auto * joint = (JointNode *)node->parent;
					if (node->isSelected) {
						active_nodes.insert(joint);
					} else {
						active_nodes.erase(joint);
					}
				}
			}

			do_picking = false;
			m_rootNode->picking_switch(1);

			CHECK_GL_ERRORS;

		}

	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == 'Q')
		{
			cout << "Q key pressed, shutting down..." << endl;
			//quit application
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == 'P')
		{
			imode = 0;
			eventHandled = true;
		}
		if (key == 'C')
		{
			if (display_circle == true) {
				display_circle = false;
			} else {
				display_circle = true;
			}
			eventHandled = true;
		}
		if (key == 'A')
		{
			resetAll();
			eventHandled = true;
		}
		if (key == 'I')
		{
			resetPosition();
			eventHandled = true;
		}
		if (key == 'O')
		{
			resetOrientation();
			eventHandled = true;
		}
		if (key == 'Z')
		{
			z_buffer_active = !z_buffer_active;
			eventHandled = true;
		}
		if (key == 'B')
		{
			bf_cull = !bf_cull;
			eventHandled = true;
		}
		if (key == 'F')
		{
			ff_cull = !ff_cull;
			eventHandled = true;
		}
		if (key == 'J')
		{
			imode = 1;
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

		return eventHandled;
}

//----------------------------------------------------------------------------------------
/**
 * Get a normalized vector for arcball manipulations
 * source: https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
 */
vec3 A3::get_arcball_vector(float x, float y) {
	int scrn_w, scrn_h;
	glfwGetWindowSize(m_window, &scrn_w, &scrn_h);
	vec3 P = vec3(1.0 * x / scrn_w * 2 - 1.0,
				  1.0 * y / scrn_h * 2 - 1.0,
				  0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1 * 1)
		P.z = sqrt(1 * 1 - OP_squared);
	else
		P = glm::normalize(P);
	return P;
}

void A3::resetAll() {
	translation = mat4();
	rotation = mat4();
	for (auto joint : active_nodes) {
		joint->xangle = 0;
		joint->yangle = 0;
	}
}

void A3::resetPosition() {
	translation = mat4();
}

void A3::resetOrientation(){
	rotation = mat4();
}

void A3::culling() {
	if (bf_cull && ff_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	} else if (bf_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else if (ff_cull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	} else {
		glDisable(GL_CULL_FACE);
	}
}