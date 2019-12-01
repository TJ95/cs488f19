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

-- reflective sphere
-- reflectiveSphere2 = gr.sphere('reflective2')
-- scene:add_child(reflectiveSphere2)
-- reflectiveSphere2:set_material(blue)
-- reflectiveSphere2:set_glossy_ind({1.0, 10, 0.0, 100})
-- reflectiveSphere2:scale(2, 2, 2)
-- reflectiveSphere2:translate(-6, -6, -5)

-- reflectiveSphere3 = gr.sphere('reflective3')
-- scene:add_child(reflectiveSphere3)
-- reflectiveSphere3:set_material(mirror2)
-- reflectiveSphere3:set_glossy_ind({1.0, 10000, 0.0, 100})
-- reflectiveSphere3:scale(2, 2, 2)
-- reflectiveSphere3:translate(6, -8, -5)

-- transparentSphere = gr.sphere('transparentSphere')
-- scene:add_child(transparentSphere)
-- transparentSphere:set_material(glass3)
-- -- transparentSphere:set_glossy_ind({0.0, 10, 1.0, 1000})
-- transparentSphere:scale(1, 1, 1)
-- transparentSphere:translate(0, -3, 4)

transparentSphere2 = gr.sphere('transparentSphere')
scene:add_child(transparentSphere2)
transparentSphere2:set_material(gold)
transparentSphere2:set_glossy_ind({1.0, 100, 1.0, 100})
transparentSphere2:scale(3,3,3)
transparentSphere2:translate(0, 0, 8)

-- transparentCylinder = gr.sphere('transparentCylinder')
-- transparentSphere:add_child(transparentCylinder)
-- transparentCylinder:set_material(water)
-- -- transparentSphere:set_glossy_ind({0.0, 10, 1.0, 1000})
-- transparentCylinder:scale(1/4, 1/4, 1/4)
-- -- transparentCylinder:translate(0.9, -2.5, 0)

-- transparentCylinder2 = gr.cylinder('transparentCylinder2')
-- scene:add_child(transparentCylinder2)
-- transparentCylinder2:set_material(glass)
-- transparentCylinder2:set_glossy_ind({0.0, 10, 1.0, 100})
-- transparentCylinder2:scale(1, 2, 1)
-- transparentCylinder2:translate(0, -7, 4)

-- transparentCone = gr.cone('transparentCone')
-- scene:add_child(transparentCone)
-- transparentCone:set_material(water)
-- -- transparentSphere:set_glossy_ind({0.0, 10, 1.0, 1000})
-- transparentCone:scale(1, 2, 1)
-- transparentCone:translate(1, -4, 4)

-- b1 = gr.cube('b1')
-- scene:add_child(b1)
-- b1:set_material(mat4)
-- b1:scale(3, 3, 3)
-- b1:translate(-5, -10, 1)

-- s1 = gr.sphere('s1')
-- scene:add_child(s1)
-- s1:set_material(mat3)
-- -- s1:scale(2, 2, 2)
-- s1:translate(-1, -6, -8)

floor = gr.plane('floor')
scene:add_child(floor)
floor:set_material(grey_wall)
--floor:set_texture("Assets/wood.png")
-- floor:set_glossy_ind({0.2, 10000, 0.0, 100})
floor:scale(10, 10, 30)
floor:translate(0, -10, 0)

backwall = gr.plane('backwall')
scene:add_child(backwall)
backwall:set_material(grey_wall)
--backwall:set_texture("Assets/orange.png")
backwall:rotate('X', 90)
backwall:scale(10, 10, 1)
backwall:translate(0, 0, -10)

leftwall = gr.plane('leftwall')
scene:add_child(leftwall)
leftwall:set_material(red_wall)
-- leftwall:set_texture(1, "Assets/brickwall.png")
-- leftwall:set_glossy_ind({1.0, 10000, 0.0, 100})
leftwall:rotate('Z', -90)
leftwall:rotate('X', 90)
leftwall:scale(10, 10, 30)
leftwall:translate(-10, 0, 0)

rightwall = gr.plane('rightwall')
scene:add_child(rightwall)
rightwall:set_material(green_wall)
-- rightwall:set_texture(1, "Assets/brickwall.png")
-- rightwall:set_glossy_ind({1.0, 10000, 0.0, 100})
rightwall:rotate('Z', 90)
rightwall:rotate('X', 90)
rightwall:scale(10, 10, 30)
rightwall:translate(10, 0, 0)

topwall = gr.plane('topwall')
scene:add_child(topwall)
topwall:set_material(white_ceiling)
-- topwall:set_texture(1, "Assets/Ceiling.png")
topwall:rotate('X', 180)
topwall:scale(10, 10, 30)
topwall:translate(0, 9.9, 0)

-- The lights
l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})
white_light = gr.light({0, 8, 10}, {0.8, 0.8, 0.8}, {1, 0, 0})
green_white_light = gr.light({0.0, 0.0, 400.0}, {0.5, 0.5, 0.5}, {1, 0, 0})
l2 = gr.light({0, 10, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})
white_spherical_light = gr.sphericallight({0, 8, 10}, {1, 1, 1}, {1, 0, 0}, 0.5)

gr.render(scene, 'room.png', 512, 512,
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_spherical_light})
