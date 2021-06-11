# Manual

There are no changes to the default build commands.

# Screenshot

screenshot.png is available in the A4 folder and is a rendering of macho-cows.

# Additional Renderings

Additional renderings are avaliable in the Assets/rendered folder. macho-cows-bb demonstrates bounding box, and nonhier-no-aa demonstrates antialiasing (the antialiased file is nonhier, and file without antialiasing is nonhier-no-aa)

In addition, nonhier-normal is rendered in false colour where the intersection normal is set as the shading colour.

See the description of "extra feature" for an explanation of the edges file.

# Extra Feature: Antialiasing

We perform adaptive antialiasing as the official extra feature. The scene is first rendered at the requested resolution. Next, each pixel is compared to its 8 neighbours and if the difference is too big, a 9-sample supersample is taken at that point. The file nonhier-edges shows the areas for which supersampling is used (in white).

To check if a pixel is different from its neighbours, we use the sum of the componentwise (by colour) variances of the pixel together with its 9 neighbours. The threshold was selected by trial and error to select most of the edges but leave blank spaces alone.

# Bounding box

Bounding box is implemented as a sphere cetered on the mesh. The mesh is assumed to have its centroid close to its origin, that is, the smallest sphere that contains every vertex is roughly cetered at the origin. Note that nonhier violates this assumption. The radius of the bounding box (sphere) is hence the maximum value of any component of any pixel. We add 10% to this to allow for floating point error.

# Background

Fractal brownian motion (fbm) using source perlin noise is used for the background. The noise value is passed through the polynomial x -> x^4 to make the clouds more clumpy. See the A1 readme for more on the fbm implementation; I copied the code from my A1 fragment shader.

# Unique Scene

A procedural phong texture is created and the unique scene demonstrates this (files: A4/Assets/sample.lua, A4/Assets/rendered/sample.png). Each procedural texture has 2 sets of phong exponent values. We use 2 different volumetric textures (real-valued functions defined on every point in space): domain-wrapped fbm and regular fbm. The sphere normal is used to index into these volumetric textures to convert them into 2D textures defined on the sphere surface. The first texture value is used to interpolate between the two phong textures. The gradient of the second texture value is used to perturb the normals, similar to a bump map (however I don't think there is a physical interpretation of the perturbation, since we take the gradient over the infinetisimal 3D volume instead of the 2D sphere surface). The procedural texture exposes two values to control these, namely the number of octaves for blending (higher octaves => more fine detail) and perturbation factor (higher factor => more perturbed). These 2 parameters are varied in a grid in the unique scene. See the A1 readme for more on the domain wrapping implementation.

# Credits

Perlin Noise code due to Ken Perlin, implementation from https://github.com/sol-prog/Perlin_Noise.

The FBM implementation is based on the one from the following source

https://thebookofshaders.com/13/

Domain wrapping code, as well as extending FBM to higher codomain dimension, is from the following source

http://www.iquilezles.org/www/articles/warp/warp.htm

Ray-Box intersection code is from

http://www.cs.utah.edu/~awilliam/box/box.pdf

I added the normal computation.

Ray-Triangle intersection code is from

https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

I added the normal computation.
