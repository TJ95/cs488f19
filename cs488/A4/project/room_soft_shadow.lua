-- Red/green test room
gold = gr.material(	 {0.9, 0.8, 0.4}, 	{0.8, 0.8, 0.4}, 	25, 0.0 ,0)
blue = gr.material(  {0.2, 0.2, 0.9}, 	{0.1, 0.1, 0.5}, 	25, 1e-5, 0)
water = gr.material( {0.05, 0.05, 0.05}, {0.6, 0.6, 0.6}, 25, 1.3333,0)
red_wall = gr.material( {0.6, 0.2, 0.3}, {0.3, 0.05, 0.05}, 25, 0.0,0)
green_wall = gr.material( {0.2, 0.6, 0.2}, {0.1, 0.05, 0.5}, 25, 0.0,0)
blue_wall = gr.material( {0.2, 0.2, 0.6}, {0.1, 0.05, 0.5}, 25, 0.0,0)
grey_wall = gr.material( {0.6, 0.623, 0.7002}, {0.3, 0.3, 0.3}, 25, 0.0,0)
reflective_wall = gr.material( {0, 0, 0}, {0.9, 0.9, 0.9}, 25, 1e-10,0)
white_ceiling = gr.material({0.9, 0.9, 0.9}, {0.5, 0.5, 0.5}, 25, 0.0,0)
white_surf = gr.material({0.1, 0.1, 0.1}, {0.9, 0.9, 0.9}, 25, 1.0,0)
mat4  = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.0,0)			--disable optics
mat1  = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0,0)  		--disable optics
mat3  = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0,0)			--disable optics
reflective = gr.material({0.0, 0.0, 0.0}, {0.7, 0.7, 0.7}, 25, 0.0,0)		--disable optics and disfusion
glossy_reflective = {1.0, 10, 0.0, 100}	
glass = gr.material( {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7}, 25, 1.5, 1)	
glass2 = gr.material( {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7},  25, 1.5,1)
glass3 = gr.material( {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7},  25, 2.5,1)			--enable optics

scene = gr.node('scene')

transparentSphere2 = gr.sphere('transparentSphere')
scene:add_child(transparentSphere2)
transparentSphere2:set_material(blue_wall)
transparentSphere2:scale(3,3,3)
transparentSphere2:translate(0, 0, 8)

floor = gr.plane('floor')
scene:add_child(floor)
floor:set_material(grey_wall)
floor:scale(10, 10, 30)
floor:translate(0, -10, 0)

backwall = gr.plane('backwall')
scene:add_child(backwall)
backwall:set_material(grey_wall)
backwall:rotate('X', 90)
backwall:scale(10, 10, 1)
backwall:translate(0, 0, -10)

leftwall = gr.plane('leftwall')
scene:add_child(leftwall)
leftwall:set_material(red_wall)
leftwall:rotate('Z', -90)
leftwall:rotate('X', 90)
leftwall:scale(10, 10, 30)
leftwall:translate(-10, 0, 0)

rightwall = gr.plane('rightwall')
scene:add_child(rightwall)
rightwall:set_material(green_wall)
rightwall:rotate('Z', 90)
rightwall:rotate('X', 90)
rightwall:scale(10, 10, 30)
rightwall:translate(10, 0, 0)

topwall = gr.plane('topwall')
scene:add_child(topwall)
topwall:set_material(white_ceiling)
topwall:rotate('X', 180)
topwall:scale(10, 10, 30)
topwall:translate(0, 9.9, 0)

-- The lights
l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})
white_light = gr.light({0, 8, 10}, {0.8, 0.8, 0.8}, {1, 0, 0})
green_white_light = gr.light({0.0, 0.0, 400.0}, {0.5, 0.5, 0.5}, {1, 0, 0})
l2 = gr.light({0, 10, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})
white_spherical_light = gr.sphericallight({0, 8, 10}, {1, 1, 1}, {1, 0, 0}, 0.5)

gr.render(scene, 'room_soft_shadowing.png', 512, 512,
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_spherical_light})
