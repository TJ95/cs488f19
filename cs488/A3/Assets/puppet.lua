-- puppet.lua
-- create the mono puppet

rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

-------------------------------------------------------------------------
-- TORSO & HIP & SHOULDER

-- node #0, the torso
torso = gr.mesh('sphere','torso')
rootnode:add_child(torso)
torso:set_material(white)
torso:scale(0.5,1.0,0.5)

-- node #1, the shoulders
shoulder = gr.mesh('sphere','shoulder')
torso:add_child(shoulder)
shoulder:set_material(blue)
shoulder:scale(2.0,1.0,2.0)
shoulder:scale(0.8,0.2,0.5)
shoulder:translate(0.0,0.8,0.0)

-- joint #1, torse-hips
waist_joint = gr.joint('waist_joint', {-10, 0, 10}, {-45, 0, 45})
torso:add_child(waist_joint)
waist_joint:translate(0.0,-0.8,0.0)

-- node #2, the hips
hips = gr.mesh('sphere','hips')
waist_joint:add_child(hips)
hips:set_material(blue)
hips:scale(2.0,1.0,2.0)
hips:scale(0.6,0.2,0.5)

-------------------------------------------------------------------------
-- LEFT ARM

-- joint #2, shoulder-left-arm
shoulder_joint_L = gr.joint('shoulder_joint_L', {-190, 0, 60}, {0, 0, 0})
shoulder:add_child(shoulder_joint_L)
shoulder_joint_L:scale(1/0.8,1/0.2,2.0)
shoulder_joint_L:translate(-0.98,0.0,0.0)

-- node #3, the upper left arm
upper_L = gr.mesh('sphere','upper_L')
shoulder_joint_L:add_child(upper_L)
upper_L:set_material(blue)
upper_L:scale(0.2,0.5,0.2)
upper_L:translate(0.0,-0.5,0.0)

-- joint #3, upper-forearm LEFT
elbow_joint_L = gr.joint('elbow_joint_L', {-160, 0, 0}, {0, 0, 0})
upper_L:add_child(elbow_joint_L)
elbow_joint_L:scale(1/0.2,2.0,1/0.2)
elbow_joint_L:translate(0.0,-0.9,0.0)

-- node #4, the left forearm
forearm_L = gr.mesh('sphere','forearm_L')
elbow_joint_L:add_child(forearm_L)
forearm_L:set_material(blue)
forearm_L:scale(0.2,0.5,0.2)
forearm_L:scale(0.8,0.8,0.8)
forearm_L:translate(0.0,-0.4,0.0)

-- joint #4, forearm-hand LEFT
wrist_joint_L = gr.joint('wrist_joint_L', {-20, 0, 20}, {-60, 0, 60})
forearm_L:add_child(wrist_joint_L)
wrist_joint_L:scale(1/0.2,2.0,1/0.2)
wrist_joint_L:translate(0.0,-2.0,0.0)

-- node #5, the left hand
hand_L = gr.mesh('sphere','hand_L')
wrist_joint_L:add_child(hand_L)
hand_L:scale(0.2,0.2,0.2)
hand_L:set_material(blue)
hand_L:translate(0.0,0.5,0.0)

-------------------------------------------------------------------------
-- RIGHT ARM

-- joint #5, shoulder-left-arm
shoulder_joint_R = gr.joint('shoulder_joint_R', {-190, 0, 60}, {0, 0, 0})
shoulder:add_child(shoulder_joint_R)
shoulder_joint_R:scale(1/0.8,1/0.2,2.0)
shoulder_joint_R:translate(0.98,0.0,0.0)

-- node #6, the upper left arm
upper_R = gr.mesh('sphere','upper_R')
shoulder_joint_R:add_child(upper_R)
upper_R:set_material(blue)
upper_R:scale(0.2,0.5,0.2)
upper_R:translate(0.0,-0.5,0.0)

-- joint #6, upper-forearm LEFT
elbow_joint_R = gr.joint('elbow_joint_R', {-160, 0, 0}, {0, 0, 0})
upper_R:add_child(elbow_joint_R)
elbow_joint_R:scale(1/0.2,2.0,1/0.2)
elbow_joint_R:translate(0.0,-0.9,0.0)

-- node #7, the left forearm
forearm_R = gr.mesh('sphere','forearm_R')
elbow_joint_R:add_child(forearm_R)
forearm_R:set_material(blue)
forearm_R:scale(0.2,0.5,0.2)
forearm_R:scale(0.8,0.8,0.8)
forearm_R:translate(0.0,-0.4,0.0)

-- joint #7, forearm-hand LEFT
wrist_joint_R = gr.joint('wrist_joint_R', {-20, 0, 20}, {-60, 0, 60})
forearm_R:add_child(wrist_joint_R)
wrist_joint_R:scale(1/0.2,2.0,1/0.2)
wrist_joint_R:translate(0.0,-2.0,0.0)

-- node #8, the left hand
hand_R = gr.mesh('sphere','hand_R')
wrist_joint_R:add_child(hand_R)
hand_R:scale(0.2,0.2,0.2)
hand_R:set_material(blue)
hand_R:translate(0.0,0.5,0.0)

---------------------------------------------------------------------
-- NECK & HEAD

-- joint #8, NECK
neck_joint = gr.joint('neck_joint', {-30, 0, 30}, {0, 0, 0})
shoulder:add_child(neck_joint)
neck_joint:scale(1/0.8,1/0.2,2.0)
neck_joint:translate(0.0,0.2,0.0)

-- node #9, the neck
neck = gr.mesh('cube','neck')
neck_joint:add_child(neck)
neck:set_material(white)
neck:scale(0.2,0.4,0.2)
neck:translate(0.0,0.2,0.0)

-- joint #9, head-neck
head_joint = gr.joint('head_joint', {0, 0, 0}, {-90, 0, 90})
neck:add_child(head_joint)
head_joint:scale(1.0/0.2,1/0.4,1.0/0.2)
head_joint:translate(0.0,0.2,0.0)

-- node #10, the head
head = gr.mesh('sphere','head')
head_joint:add_child(head)
head:set_material(white)
head:scale(0.3,0.3,0.3)
head:translate(0.0,0.2,0.0)

-- node #11, the nose (direction indicator)
nose = gr.mesh('sphere','nose')
head:add_child(nose)
nose:set_material(white)
nose:scale(0.2,0.2,0.2)
nose:translate(0.0,0.0,1.0)

---------------------------------------------------------------
-- LEFT LEG

-- joint #10, hip_L
hip_joint_L = gr.joint('hip_joint_L', {-120, 0, 30}, {-90, 0, 15})
hips:add_child(hip_joint_L)
hip_joint_L:scale(1/0.6,1/0.2,2.0)
hip_joint_L:translate(-0.6,0,0)

-- node #12, the left thigh
thigh_L = gr.mesh('sphere','thigh_L')
hip_joint_L:add_child(thigh_L)
thigh_L:set_material(blue)
thigh_L:scale(0.25,0.6,0.25)
thigh_L:translate(0.0,-0.5,0.0)

-- joint #11,knee_L
knee_L = gr.joint('knee_L', {0, 0, 120}, {0, 0, 0})
thigh_L:add_child(knee_L)
knee_L:scale(4.0,1/0.6,4.0)
knee_L:translate(0.0,-1.0,0.0)

-- node #13, the left calf
calf_L = gr.mesh('sphere','calf_L')
knee_L:add_child(calf_L)
calf_L:set_material(blue)
calf_L:scale(0.25,0.6,0.25)
calf_L:scale(0.8,1.0,0.8)
calf_L:translate(0.0,-0.5,0.0)

-- joint #12, ankle_L
ankle_L = gr.joint('ankle_L', {-20, 0, 20}, {-60, 0, 10})
calf_L:add_child(ankle_L)
ankle_L:scale(4.0,1/0.6,4.0)
ankle_L:translate(0.0,-0.8,0.0)

-- node #14, the left foot
foot_L = gr.mesh('sphere','foot_L')
ankle_L:add_child(foot_L)
foot_L:set_material(blue)
foot_L:scale(0.25,0.6,0.25)
foot_L:scale(0.7,0.2,1.5)
foot_L:translate(0.0,-0.1,0.2)

---------------------------------------------------------------
-- LEFT LEG

-- joint #10, hip_R
hip_joint_R = gr.joint('hip_joint_R', {-120, 0, 30}, {-15, 0, 90})
hips:add_child(hip_joint_R)
hip_joint_R:scale(1/0.6,1/0.2,2.0)
hip_joint_R:translate(0.6,0,0)

-- node #12, the left thigh
thigh_R = gr.mesh('sphere','thigh_R')
hip_joint_R:add_child(thigh_R)
thigh_R:set_material(blue)
thigh_R:scale(0.25,0.6,0.25)
thigh_R:translate(0.0,-0.5,0.0)

-- joint #11,knee_L
knee_R = gr.joint('knee_R', {0, 0, 120}, {0, 0, 0})
thigh_R:add_child(knee_R)
knee_R:scale(4.0,1/0.6,4.0)
knee_R:translate(0.0,-1.0,0.0)

-- node #13, the left calf
calf_R = gr.mesh('sphere','calf_R')
knee_R:add_child(calf_R)
calf_R:set_material(blue)
calf_R:scale(0.25,0.6,0.25)
calf_R:scale(0.8,1.0,0.8)
calf_R:translate(0.0,-0.5,0.0)

-- joint #12, ankle_L
ankle_R = gr.joint('ankle_R', {-20, 0, 20}, {-10, 0, 60})
calf_R:add_child(ankle_R)
ankle_R:scale(4.0,1/0.6,4.0)
ankle_R:translate(0.0,-0.8,0.0)

-- node #14, the left foot
foot_R = gr.mesh('sphere','foot_R')
ankle_R:add_child(foot_R)
foot_R:set_material(blue)
foot_R:scale(0.25,0.6,0.25)
foot_R:scale(0.7,0.2,1.5)
foot_R:translate(0.0,-0.1,0.2)


-- move the puppet to a more seeable distance
rootnode:translate(0.0, 0.5, -6.0)

return rootnode
