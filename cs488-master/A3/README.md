# Compilation

There are no changes relative to the default gmake/make combination.

# Quickstart

When puppet.lua is displayed, we are initially in raster mode. Try enabling raymarch mode from the options menu. Note that there appears to be gaps in the puppet; this is expected and disappears in the other mode (e.g. in the screenshot).

To test joints, it is recommended to stay in raster mode and select all the joints (every joint is a cylinder). Then, hold the middle and right mouse buttons down while dragging the mouse. For e.g., moving the mouse down should make the neck coil into a loop.

# Manual

Joints work differently from the spec as follows: when the middle button is held down (in Joint mode), the mouse's relative Y displacement translates to rotation about the X axis. The mouse's relative X displacement is discarded. When the right mouse button is held down, the mouse's relative X displacement translates to rotation about the Y axis. These are applied in the sequence stated.

Initially, the puppet primarily extends in the Y direction. Any joint can be used to orient the neck above that joint as follows: consider the head to be a point which is constrained to move on a sphere, and take the y axis to point to a virtual "north pole" of that sphere; first use the middle button to manipulate the head's "latitude", then use the right button to manipulate the "longitude".

Note that the initial position places each joint in gimbal lock, hence instantaneously only having 1 degree of freedom (again, treating the head as a point); the position of the head on the virtual sphere determines the head's orientation; hence, the joint closest to the head is treated specially to have only 1 degree of freedom to allow easier posing.

The position/orientation interaction mode rotates the puppet around the root node's coordinate system. Hence a3mark will rotate around the sphere, while alolan exeggutor rotates around its body.

Also note that picking should work in every mode.

# Model hierarchy

The neck is constructed as follows: each segment starts a joint (2 degrees of freedom). A geometry node (the cylinder) is attached to the joint. An extrusion node is also attached to the joint, which translates away from the joint. The next segment starts from the extrusion node.

The lowest neck segment is attached via a joint to alolan exeggutor's body. The body is rigid (can only be manipulated via rotating the entire puppet).

Alolan exeggutor's heads and bulbs are rigidly attached to the last neck segment (this can be edited via puppet.lua but not via joint manipulation).

# Shading

Raymarch mode switches to a fragment shader that constructs a signed distance field (sdf) representing a scene with the same geometry as the model. The model surface is the set of points p for which scene(p) = 0 where `scene` is the function on line 208. The fragment shader then performs raymarching on the sdf and uses the same illumination model as in Phong.fs to compute each pixel colour. Surface normals are computed numerically. Hence the yellow portion of the scene (alolan exeggutor's heads) should look the same in both modes. The brown portion of the scene uses a soft minimum function to blend alolan exeggutor's neck, head, body and tail. The absolute (non-hierarchical) position and orientation of each sdf primitive (corresponding to a GeometryNode) is passed to the fragment shader via uniforms.

The raster shader is used to render the selected primitives as a flat transparent shape, which is then overlayed onto the scene. Picking is also done the same way.

# Limitations

Raymarching mode is not supported for any other models.

Alolan exeggutor's legs, eyes, mouth, and fourth head (on the tail) are not modeled due to time constraints. Similarly the leaves (green) are only done in the fragment shader.

The gaps in the model are due to the cylinder mesh I used not having caps, and I couldn't model the caps in time. Hence there is a difference between the representation of the puppet stored in GeometryNode / mesh vertices and the representation via the sdf. Similarly the neck in the sdf actually smoothly interpolates between two radii (can be edited in puppet.lua) which is not done in the mesh.

Materials and lights in the raymarch shader are hardcoded in, although like in Phong.fs they could be passed in via uniforms. Due to the surface being defined implicitly the materials have to be hardcoded in as well.

# Data structure changes

The lua importer is modified to pass the geometry node associated with a joint node via set_selection. Optional auxiliary data is also passed and sent to the fragment shader.

The SceneNode hierarchy has virtual functions added for all rendering.

Undo/redo is supported via two stacks, containing the potential undo actions and the potential redo actions; undo/redo moves the top item from a stack to the other. We maintain the invariant that when a joint manipulation drag is not in progress, the top of the undo stack should contain the current joint state.

# Acknowledgements

Shader code taken from from https://www.shadertoy.com/view/Xls3R7, subsequently modified to match the shading done in Phong.fs. Most SDF functions taken from https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm.

# Machine used

38
