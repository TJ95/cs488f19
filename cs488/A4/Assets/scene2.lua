-- Red/green test room
gold = gr.material(	 {0.9, 0.8, 0.4}, 	{0.8, 0.8, 0.4}, 	25, 0.2 ,0)
blue = gr.material(  {0.2, 0.2, 0.9}, 	{0.1, 0.1, 0.5}, 	25, 1e-5, 0)
water = gr.material( {0.05, 0.05, 0.05}, {0.6, 0.6, 0.6}, 25, 1.3333,0)
red_wall = gr.material( {0.6, 0.2, 0.3}, {0.3, 0.05, 0.05}, 25, 0.0,0)
red_metal = gr.material( {0.8, 0.2, 0.3}, {0.3, 0.05, 0.05}, 25, 0.0,0)
green_wall = gr.material( {0.2, 0.6, 0.2}, {0.1, 0.05, 0.5}, 25, 0.0,0)
blue_wall = gr.material( {0.2, 0.2, 0.6}, {0.1, 0.05, 0.5}, 25, 0.0,0)
white_wall = gr.material( {0.9, 0.9, 0.9}, {0.1, 0.05, 0.5}, 25, 0.0,0)
grey_wall = gr.material( {0.6, 0.623, 0.7002}, {0.3, 0.3, 0.3}, 25, 0.0,0)
reflective_wall = gr.material( {0, 0, 0}, {0.9, 0.9, 0.9}, 25, 0.18,0)
white_ceiling = gr.material({0.9, 0.9, 0.9}, {0.5, 0.5, 0.5}, 25, 0.0,0)
nocolor = gr.material({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}, 25, 0.0,0)
nocolor_trans = gr.material({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}, 25, 0.0,1)
white_surf = gr.material({0.1, 0.1, 0.1}, {0.9, 0.9, 0.9}, 25, 1.0,0)
mat4  = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.0,0)			--disable optics
mat1  = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.0,0)  		--disable optics
mat3  = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0,0)			--disable optics
reflective = gr.material({0.0, 0.0, 0.0}, {0.7, 0.7, 0.7}, 25, 0.0,0)		--disable optics and disfusion
glossy_reflective = {1.0, 10, 0.0, 100}	
glass = gr.material( {0.05, 0.05, 0.05}, {0.7, 0.7, 0.7}, 25, 1.1, 1)	
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

Basketball = gr.sphere('Basketball')
scene:add_child(Basketball)
Basketball:set_material(nocolor)
Basketball:set_texture("Assets/Basketball_texture.png")
--Basketball:set_bumpmap("Assets/Basketball_bump.png")
--Basketball:set_glossy_ind({1.0, 100, 1.0, 100})
Basketball:rotate('Z', 30)
Basketball:rotate('Y', 70)
Basketball:rotate('X', 90)
Basketball:scale(2,2,2)
Basketball:translate(-2, -8, -1)

Basketball2 = gr.sphere('Basketball2')
scene:add_child(Basketball2)
Basketball2:set_material(nocolor)
Basketball2:set_texture("Assets/Basketball_texture.png")
--Basketball:set_bumpmap("Assets/Basketball_bump.png")
--Basketball:set_glossy_ind({1.0, 100, 1.0, 100})
Basketball2:rotate('Z', 30)
Basketball2:rotate('Y', 70)
Basketball2:rotate('X', 90)
Basketball2:scale(2,2,2)
Basketball2:translate(12, -8, 15)

-- LArry OB
lob = gr.sphere('lob')
scene:add_child(lob)
lob:set_material(gold)
--lob:set_bumpmap("Assets/Basketball_bump.png")
lob:set_glossy_ind({1.0,100,1.0,100})
lob:translate(4,-5, 3)

lob_cylinder = gr.cylinder('lob_cylinder')
lob:add_child(lob_cylinder)
lob_cylinder:set_material(gold)
lob_cylinder:set_glossy_ind({1.0,100,1.0,100})
lob_cylinder:scale(0.3,0.5,0.3)
lob_cylinder:translate(0,-1,0)

lob_cone = gr.cone('lob_cone')
lob:add_child(lob_cone)
lob_cone:set_material(gold)
lob_cone:set_glossy_ind({1.0,100,1.0,100})
lob_cone:scale(1.2,4,1.2)
lob_cone:translate(-0.7,-5.2,0)

lob_cube = gr.cube('lob_cube')
lob:add_child(lob_cube)
lob_cube:set_material(gold)
lob_cube:set_glossy_ind({1.0,100,1.0,100})
lob_cube:scale(2.5,0.6,2.5)
lob_cube:translate(-2,-5,0)

-- backboard
b1 = gr.cube('b1')
scene:add_child(b1)
b1:set_material(glass)
b1:scale(12, 8, 1)
--b1:rotate('Y', -30)
b1:translate(-6,8,-28)
-- left line
s1 = gr.plane('s1')
scene:add_child(s1)
s1:set_material(white_wall)
s1:scale(0.1, 1.0, 1.5)
s1:rotate('X',-90)
s1:translate(-2.5,10.0,-26.9)
-- top line
s2 = gr.plane('s2')
scene:add_child(s2)
s2:set_material(white_wall)
s2:scale(0.1, 1.0, 2.3)
s2:rotate('X',-90)
s2:rotate('Z',90)
s2:translate(-0.1,11.4,-26.9)
-- bot line
s3 = gr.plane('s3')
scene:add_child(s3)
s3:set_material(white_wall)
s3:scale(0.1, 1.0, 2.3)
s3:rotate('X',-90)
s3:rotate('Z',90)
s3:translate(-0.1,8.6,-26.9)
-- right line
s4 = gr.plane('s4')
scene:add_child(s4)
s4:set_material(white_wall)
s4:scale(0.1, 1.0, 1.5)
s4:rotate('X',-90)
s4:translate(2.2,10.0,-26.9)

torus1 = gr.torus('torus1')
scene:add_child(torus1)
torus1:set_material(red_metal)
torus1:scale(2,2,2)
 torus1:rotate('X', 90)
 torus1:translate(0,9,-25.1)


-- box starts here
floor = gr.plane('floor')
scene:add_child(floor)
floor:set_material(reflective_wall)
floor:set_texture("Assets/halfcourt.png")
-- floor:set_glossy_ind({0.2, 10000, 0.0, 100})
floor:rotate('Y',-90)
floor:scale(30, 30, 30)
floor:translate(0, -10, 0)

backwall = gr.plane('backwall')
scene:add_child(backwall)
backwall:set_material(red_wall)
--backwall:set_texture("Assets/black_blend_wall.png")
--backwall:set_texture("Assets/raptors_black.png")
backwall:rotate('X', 90)
backwall:scale(30, 30, 1)
backwall:translate(0, 20, -30)
--backwall:rotate('Y', -30)
--backwall:scale(3, 3, 1)


leftwall = gr.plane('leftwall')
scene:add_child(leftwall)
leftwall:set_material(nocolor)
leftwall:set_texture("Assets/raptors_black.png")
-- leftwall:set_glossy_ind({1.0, 10000, 0.0, 100})
leftwall:rotate('Z', -90)
leftwall:rotate('X', 90)
leftwall:scale(30, 30, 30)
leftwall:translate(-30, 20, -10)
--leftwall:rotate('Y', -20)

rightwall = gr.plane('rightwall')
scene:add_child(rightwall)
rightwall:set_material(green_wall)
-- rightwall:set_texture(1, "Assets/brickwall.png")
-- rightwall:set_glossy_ind({1.0, 10000, 0.0, 100})
rightwall:rotate('Z', 90)
rightwall:rotate('X', 90)
rightwall:scale(30, 30, 30)
rightwall:translate(30, 0, 0)

topwall = gr.plane('topwall')
scene:add_child(topwall)
topwall:set_material(white_ceiling)
-- topwall:set_texture(1, "Assets/Ceiling.png")
topwall:rotate('X', 180)
topwall:scale(30, 30, 30)
topwall:translate(0, 30, 0)

scene:rotate('Y',-20)
scene:translate(0,0,-6)

-- The lights
l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})
white_light = gr.light({0, 28, 20}, {0.8, 0.8, 0.8}, {1, 0, 0})
green_white_light = gr.light({0.0, 0.0, 400.0}, {0.5, 0.5, 0.5}, {1, 0, 0})
l2 = gr.light({0, 10, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})
white_spherical_light = gr.sphericallight({0, 8, 10}, {1, 1, 1}, {1, 0, 0}, 0.5)
white_spherical_light2 = gr.sphericallight({0, 28, 28}, {1, 1, 1}, {1, 0, 0}, 0.5)

gr.render(scene, 'scene2.png', 256, 256,
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
