// Fall 2019

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;


static const size_t DIM = 16;
// scaling factor of the walls
int wall_height = 1;
// used to help determining the color editor's target
int ctarget = 0;
// the maze
Maze m(DIM);
// 2D coordinates of the avatar 
glm::vec2 aPos(-1,-1);
// the previously recorded X value of the mouse position
double prev_mouse = -1;
// Records mouse movement
int movement = 0;
// Booleans
// ------------------------------------------
// If a maze exists on the grid
bool MazeDug = false;
// If the cube is in 'eating mode' (Lshift key pressed)
bool eating = false;
// If the mouse LB is pressed
bool mouse_pressed = false;
// If persistence is triggered
bool gravity_spinning = false;
// ------------------------------------------
// default object colors
// ------------------------------------------
static float default_colors[3][3] = {
	0.7f, 0.7f, 0.7f, // floor
	0.97f, 0.5f, 0.18f, // cubes/walls
	1.0f, 0.87f, 0.2f  // avatar
};

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
	m_world_rotation = 0.0f;
	m_world_size = 1.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
	initFloor();
	initCube();
	initAvatar(-1,0,-1);
	// Initialize default colors
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			element_colors[i][j] = default_colors[i][j];
		}
	}


	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initFloor()
{
	// creating a floor floating slightly above the grid
	float verts[12] = {
		0, 0.01, 0,
		DIM, 0.01, 0,
		DIM, 0.01, DIM,
		0, 0.01, DIM};

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_floor_vao);
	glBindVertexArray(m_floor_vao);

	// Create the cube vertex buffer
	glGenBuffers(1, &m_floor_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float),
				 verts, GL_STATIC_DRAW);
	
	// Create the cube elements buffer
	glGenBuffers(1, &m_floor_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_floor_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int),
				 inds, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	//delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initCube()
{
	float verts[24] = {
		0, 0, 0,
		1, 0, 0,
		1, 0, 1,
		0, 0, 1,
		0, 1, 0,
		1, 1, 0,
		1, 1, 1,
		0, 1, 1
	};

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_cube_vao);
	glBindVertexArray(m_cube_vao);

	// Create the cube vertex buffer
	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float),
				 verts, GL_STATIC_DRAW);

	glGenBuffers(1, &m_cube_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int),
				 inds, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	//delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initAvatar(float x, float y, float z)
{
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_avatar_vao);
	glBindVertexArray(m_avatar_vao);

	float verts[24];
	// Compute the coordinates for current avatar
	verts[0] = x + 0.15;  verts[1] = y;        verts[2] = z + 0.15;
	verts[3] = x + 0.15;  verts[4] = y;        verts[5] = z + 0.85;
	verts[6] = x + 0.85;  verts[7] = y;        verts[8] = z + 0.85;
	verts[9] = x + 0.85;  verts[10] = y;       verts[11] = z + 0.15;
	verts[12] = x + 0.15; verts[13] = y + 0.7; verts[14] = z + 0.15;
	verts[15] = x + 0.15; verts[16] = y + 0.7; verts[17] = z + 0.85;
	verts[18] = x + 0.85; verts[19] = y + 0.7; verts[20] = z + 0.85; 
	verts[21] = x + 0.85; verts[22] = y + 0.7; verts[23] = z + 0.15;

	// Create the cube vertex buffer
	glGenBuffers(1, &m_avatar_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_avatar_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float),
				 verts, GL_STATIC_DRAW);

	glGenBuffers(1, &m_avatar_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_avatar_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int),
				 inds, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	// limiting the scaling boundary so the maze is in reasonable size
	m_world_size = glm::clamp(m_world_size, 0.05f, 4.0f);

	// if persistence is triggered, keep 80% of its "momentum" and keep the rotation going
	// until next LMB click
	if ( gravity_spinning && !mouse_pressed ) {
	    m_world_rotation += glm::radians(movement*0.8f);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset" ) ) {
			reset();
		}
		if( ImGui::Button( "Dig Maze" ) ) {
			dig();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::PushID( 1 );
		ImGui::PushID( 2 );
		ImGui::ColorEdit3( "##Colour", element_colors[ctarget] );
		if( ImGui::RadioButton( "Floor", &current_col, 0 ) ) {
			ctarget = 0;
		}
		if( ImGui::RadioButton( "Walls", &current_col, 1 ) ) {
			ctarget = 1;
		}
		if( ImGui::RadioButton( "Avatar", &current_col, 2 ) ) {
			ctarget = 2;
		}
		
		ImGui::PopID();
		ImGui::PopID();
		ImGui::PopID();

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W *= glm::rotate(mat4(), glm::radians(m_world_rotation), vec3(0.0f,1.0f,0.0f));
	W *= glm::scale(mat4(), vec3(m_world_size));
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// draw the floor
		glBindVertexArray( m_floor_vao );
		glUniform3f(col_uni,
					element_colors[0][0],
					element_colors[0][1],
					element_colors[0][2]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Draw the cubes
		glBindVertexArray( m_cube_vao );
		glUniform3f(col_uni,
					element_colors[1][0],
					element_colors[1][1],
					element_colors[1][2]);
		mat4 M = W;
		for (int i = 0; i < DIM; i++) {
			for (int j = 0; j < DIM; j++) {
				M = W;
				M = glm::translate(M, vec3(j, 0, i));
				M = glm::scale(M, vec3(1,wall_height,1));
				glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(M));
				if (MazeDug && m.getValue(i,j) == 1) {
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				}
			}
		}
		glBindVertexArray(0);
		glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

		// Draw avatar at its current location
		glBindVertexArray( m_avatar_vao );
		glUniform3f(col_uni,
					element_colors[2][0],
					element_colors[2][1],
					element_colors[2][2]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); 

		// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);
	int d;
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if ( mouse_pressed ) {

			if (prev_mouse != -1) { // clicking outside of the widow should not trigger rotation

				d = xPos - prev_mouse;
				movement = d;
				prev_mouse = xPos;

				m_world_rotation += glm::radians(d*5.0f);

			} else if (prev_mouse == -1) {

				prev_mouse = xPos;

			} 
		} 
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
			// flip the mouse_essed indicator and stop any persistence rotation
			mouse_pressed = true;
			gravity_spinning = false;
			movement = 0;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_RELEASE) {
			// reset the prev mouse position and flip the pressed indicator
			prev_mouse = -1;
			mouse_pressed = false;
			// if movement is detected when mouse release is fired, trigger persistence by
			// flipping on the boolean value
			if ( movement > 1 || movement < -1 ) {
                gravity_spinning = true;
			}
		}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);
	// Zoom in or out.
    m_world_size += yOffSet * 0.06;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		if (key == 'Q')
		{
			cout << "Q key pressed, shutting down..." << endl;
			//quit application
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		// Event 'D', generate a new maze
		if (key == 'D')
		{
			cout << "Digging new maze..." << endl;
			dig();
			eventHandled = true;
		}
		// Event 'R', reset everything to default
		if (key == 'R') {
			cout << "It's REWIND time!" << endl;
			reset();
			eventHandled = true;
		}
		// Event 'SPACE BAR', grow wall size by 1
		if (key == 32)
		{
			if (MazeDug)
			{
				wall_edit(1);
			}
			eventHandled = true;
		}
		// Event 'SPACE BAR', grow wall size by 1
		if (key == 259)
		{
			if (MazeDug && wall_height > 1)
			{
				wall_edit(-1);
			}
			if (wall_height == 1)
			{
				cout << "don't shrink into the ground!" << endl;
			}
			eventHandled = true;
		}
		// if shift key is pressed (mods == 1), turn on wall-eating mode
		// Move avatar up by 1 unit
		if (key == 265)
		{
			if (mods == 1)
				eating = true;
			translate_avatar(1);
			eventHandled = true;
		}
		// Move avatar down by 1 unit
		if (key == 264)
		{
			if (mods == 1)
				eating = true;
			translate_avatar(2);
			eventHandled = true;
		}
		// Move avatar right by 1 unit
		if (key == 262)
		{
			if (mods == 1)
				eating = true;
			translate_avatar(3);
			eventHandled = true;
		}
		// Move avatar left by 1 unit
		if (key == 263)
		{
			if (mods == 1)
				eating = true;
			translate_avatar(4);
			eventHandled = true;
		}
	}
	// turn of wall-eating mode upon releasing of Lshift
	if (action == GLFW_RELEASE && key == 340) {
		eating = false;
		eventHandled = true;
	}

	return eventHandled;
}

// Dig a maze, place avatar at the starting position
void A1::dig()
{
	if (!MazeDug) {
		m.digMaze();
		for (int i = 0; i < DIM; i++) {
			if (m.getValue(0,i) != 1) {
				aPos = vec2(0,i+1);
				teleport(i, 0, 0);
				break;
			}
		} 
		MazeDug = true;
	} else {
		wall_height = 1;
		m.reset();
		m.digMaze();
		for (int i = 0; i < DIM; i++)
		{
			if (m.getValue(0, i) != 1)
			{
				aPos = vec2(0, i+1);
				teleport(i, 0, 0);
				break;
			}
		}
	}
}

void A1::wall_edit(int i)
{
	wall_height+=i;
}

void A1::teleport(float x, float y, float z) {

	// Compute the coordinates for current avatar
	ava[0] = x + 0.15; ava[1] = y; ava[2] = z + 0.15;
	ava[3] = x + 0.15; ava[4] = y; ava[5] = z + 0.85;
    ava[6] = x + 0.85; ava[7] = y; ava[8] = z + 0.85;
	ava[9] = x + 0.85; ava[10] = y; ava[11] = z + 0.15;
	ava[12] = x + 0.15; ava[13] = y + 0.7;ava[14] = z + 0.15;
	ava[15] = x + 0.15;ava[16] = y + 0.7;ava[17] = z + 0.85;
	ava[18] = x + 0.85;ava[19] = y + 0.7;ava[20] = z + 0.85;
	ava[21] = x + 0.85;ava[22] = y + 0.7;ava[23] = z + 0.15;
	
	aPos = vec2(x,z);

	glBindBuffer(GL_ARRAY_BUFFER, m_avatar_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float),
				 ava, GL_STATIC_DRAW);
}
// translates the avatar in 4 directions, if no wall is in the way
void A1::translate_avatar(int d)
{
	// load the x,z pos of the avatar
	float x = aPos[0];
	float z = aPos[1];
	// checking if avatar is exiting the maze
	// checking if avatar is on outer perimeter
	bool LR_rim = false;
	bool UD_rim = false;
	if (x == -1 || x == 16) LR_rim = true;
	if (z == -1 || z == 16) UD_rim = true;

	switch (d) {
		case 1: // up
			if (z > -1 && (eating || m.getValue(z-1,x)!=1 || z == 0 || LR_rim)) {
				teleport(x, 0, z - 1);
				aPos = vec2(x,z-1);
			}
			break;
		case 2:  // down
			if (z < DIM && (eating || m.getValue(z+1,x)!=1 || z == 15 || LR_rim)) {
				teleport(x,0,z+1);
				aPos = vec2(x,z+1);
			}
			break;
		case 3: // right
			if (x < DIM && (eating || m.getValue(z,x+1)!=1 || UD_rim)) {
				teleport(x+1, 0, z);
				aPos = vec2(x+1, z);
			}
			break;
		case 4: // left
			if (x > -1 && (eating || m.getValue(z,x-1)!=1 || UD_rim)) {
				teleport(x-1, 0, z);
				aPos = vec2(x-1, z);
			}
			break;
	}
	if (eating) {
		m.setValue(aPos[1],aPos[0],0);
	}
}

void A1::reset() {
	m.reset();
	MazeDug = false;
	aPos = vec2(-1,-1);
	teleport(-1,0,-1);
	wall_height = 1;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			element_colors[i][j] = default_colors[i][j];
		}
	}
	m_world_rotation = 0.0f;
    m_world_size = 1.0f;
    movement = 0;
    gravity_spinning = 0;
    mouse_pressed = false;
}