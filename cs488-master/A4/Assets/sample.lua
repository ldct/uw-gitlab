-- A simple scene with some miscellaneous geometry.

OCTAVES = {1,5,100}
BLENDS = {0.0, 0.1, 0.5}

mats = {}
for i=1,3 do
  mats[i] = {}
  for j=1,3 do
	mats[i][j] = gr.procedural_material({1.0, 0.65, 0.0}, {1.0, 0.75, 0.25}, 25, {0, 0.5, 0.0}, {0, 0, 0}, 25, BLENDS[i], OCTAVES[j])
  end
end

scene_root = gr.node('root')

for i=1,3 do
	for j=1,3 do
		s = gr.nh_sphere('s' .. i .. j, {(i-2)*250, (j-2)*250, -400}, 100)
		scene_root:add_child(s)
		s:set_material(mats[i][j])
	end
end

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
