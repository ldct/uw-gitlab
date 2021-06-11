rootNode = gr.node('root')

yellow = gr.material({0.7*0.984,0.7*0.965,0.0},{0.1*0.984,0.1*0.965,0.0},16)
brown = gr.material({0.7*0.5,0.7*0.35,0.7*0.05},{0.1*0.5,0.1*0.35,0.1*0.05},16)


white = gr.material({0.7, 0.7, 0.7}, {0.3, 0.3, 0.3}, 16)

rootNode:rotate('y', 0)
rootNode:translate(0.0, -3.0, -10.0)

body = gr.node('body')
rootNode:add_child(body)

c1 = gr.mesh('sphere', 'body_main')
c1:scale(0.9,0.9,0.9)
c1:set_material(brown)
body:add_child(c1)

local NECK_HEIGHT={
    [0]=0.7,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    1.2
}

local NECK_RADIUS={
    [0]=0.5,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25,
    0.25
}

function make_segment(name, extrusion, from, max_x)
    extrude = gr.node(name .. '_extrude')
    extrude:translate(0.0,extrusion,0.0)
    from:add_child(extrude)
    node = gr.joint(name, {0,0,max_x},{-90,0,90})
    extrude:add_child(node)
    return node
end

function make_geometry(name, ra, rb, from, height)
    node = gr.mesh('cylinder', name, rb/ra)
    node:scale(ra, height, ra)
    node:set_material(brown)
    from:add_child(node)
    from:set_selection(node)
end

neck_from_0 = make_segment('neck_from_0', 0.8, rootNode, 60)
neck_from_1 = make_segment('neck_from_1', NECK_HEIGHT[0]+0.15, neck_from_0, 60)
neck_from_2 = make_segment('neck_from_2', NECK_HEIGHT[1]+0.15, neck_from_1, 60)
neck_from_3 = make_segment('neck_from_3', NECK_HEIGHT[1]+0.15, neck_from_2, 60)
neck_from_4 = make_segment('neck_from_4', NECK_HEIGHT[1]+0.15, neck_from_3, 60)
neck_from_5 = make_segment('neck_from_5', NECK_HEIGHT[1]+0.15, neck_from_4, 60)
neck_from_6 = make_segment('neck_from_6', NECK_HEIGHT[1]+0.15, neck_from_5, 60)
neck_from_7 = make_segment('neck_from_7', NECK_HEIGHT[1]+0.15, neck_from_6, 60)
neck_from_8 = make_segment('neck_from_8', NECK_HEIGHT[1]+0.15, neck_from_7, 0)

make_geometry('neck_0', NECK_RADIUS[0], NECK_RADIUS[1], neck_from_0, NECK_HEIGHT[0])
make_geometry('neck_1', NECK_RADIUS[1], NECK_RADIUS[2], neck_from_1, NECK_HEIGHT[1])
make_geometry('neck_2', NECK_RADIUS[2], NECK_RADIUS[3], neck_from_2, NECK_HEIGHT[2])
make_geometry('neck_3', NECK_RADIUS[3], NECK_RADIUS[4], neck_from_3, NECK_HEIGHT[3])
make_geometry('neck_4', NECK_RADIUS[4], NECK_RADIUS[5], neck_from_4, NECK_HEIGHT[4])
make_geometry('neck_5', NECK_RADIUS[5], NECK_RADIUS[6], neck_from_5, NECK_HEIGHT[5])
make_geometry('neck_6', NECK_RADIUS[6], NECK_RADIUS[7], neck_from_6, NECK_HEIGHT[6])
make_geometry('neck_7', NECK_RADIUS[7], NECK_RADIUS[8], neck_from_7, NECK_HEIGHT[7])
make_geometry('neck_8', NECK_RADIUS[8], NECK_RADIUS[9], neck_from_8, NECK_HEIGHT[8])

neck_from_head = gr.node('neck_from_head')
neck_from_head:translate(0.0, NECK_HEIGHT[8], 0.0)
neck_from_8:add_child(neck_from_head)

head_bulb = gr.mesh('sphere', 'head_bulb')
head_bulb:scale(0.5,0.5,0.5)
head_bulb:set_material(brown)
neck_from_head:add_child(head_bulb)

h1 = gr.mesh('sphere', 'h1')
h1:set_material(yellow)
h1:scale(0.3,0.33,0.3)
h1:translate(0.57,0.0,0.0)
h1:rotate('y', -20)
neck_from_head:add_child(h1)

h2 = gr.mesh('sphere', 'h2')
h2:set_material(yellow)
h2:scale(0.3,0.33,0.3)
h2:translate(0.57,0.0,0.0)
h2:rotate('y', -90)
neck_from_head:add_child(h2)

h3 = gr.mesh('sphere', 'h3')
h3:set_material(yellow)
h3:scale(0.3,0.33,0.3)
h3:translate(0.57,0.0,0.0)
h3:rotate('y', -160)
neck_from_head:add_child(h3)

return rootNode
