// Winter 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "PerlinNoise.hpp"

#include <cassert>
#include <algorithm>
#include <vector>
#include <map>
#include <iomanip>

#include "cs488-framework/MathUtils.hpp"

#include "GeometryNode.hpp"

using namespace std;
using namespace glm;

extern vector<string> args;

constexpr double SUPERSAMPLE_THRESHOLD = 0.001;

ostream & operator<<(ostream & o, vec3 v) {
  return o << "vec3(" << v.x << "," << v.y << "," << v.z << ")";
}

int num_traces = 0;

PerlinNoise perlinNoise = PerlinNoise();

float fbm3 (vec3 st, int octaves) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * perlinNoise.noise(st.x, st.y, st.z);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

float fbm2 (vec2 st, int octaves) {
	// https://thebookofshaders.com/13/
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    for (int i = 0; i < octaves; i++) {
        value += amplitude * perlinNoise.noise(st.x, st.y, 0.0);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

vec3 colour_at(
	const vec3& ro,
	const vec3& rd,
	const SceneNode* root,
	const list<Light *>& lights,
	const vec3& ambiant
) {

	num_traces++;

	vec3 normal;
	Material* material = nullptr;

	float t = root->intersects(ro, rd, &normal, &material);

	// background
	if (t < 0.0) {
		return vec3(0.0, 0.5, 0.5);
	}

	auto p = ro + t*rd;
	assert(material);
	return material->get_colour(lights, normal, p, root, ro, ambiant);

}

vec3 sample(
	int offset,
	size_t w, size_t h,
	int x, int y,
	const vec3& eye,
	const vec3& topleft,
	const vec3& img_right,
	const vec3& img_down,
	const SceneNode* root,
	const list<Light *>& lights,
	const vec3& ambiant
) {
	int side = 2*offset+1;
	int num_samples = side*side;

	vec3 ret;

	for (int dx = -offset; dx <= offset; dx++) {
		for (int dy = -offset; dy <= offset; dy++) {
			float step = 1.0f/side;
			float sx = x+dx*step;
			float sy = y+dy*step;

			float u = 2.0*sx / w;
			float v = 2.0*sy / h;

			auto rd = topleft + u*img_right + v*img_down;
			auto s_colour = colour_at(eye, rd, root, lights, ambiant);

			ret += (1.0f / num_samples) * s_colour;
		}
	}
	return ret;

}

void A4_Render(
		// What to render
		SceneNode* root,

		// Image to write to, set to a given width and height
		Image& image,

		// Viewing parameters
		const glm::vec3& eye,
		const glm::vec3& view,
		const glm::vec3& up,
		double fovy,

		// Lighting parameters
		const glm::vec3& ambient,
		const list<Light *>& lights
) {

  cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
		  "\t" << "args=" << args.size() <<
          "\t" << "eye:  " << glm::to_string(eye) << endl <<
		  "\t" << "view: " << glm::to_string(view) << endl <<
		  "\t" << "up:   " << glm::to_string(up) << endl <<
		  "\t" << "fovy: " << fovy << endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << endl <<
		  "\t" << "lights{" << endl;

	for(const Light * light : lights) {
		cout << "\t\t" <<  *light << endl;
	}
	cout << "\t}" << endl;
	 cout <<")" << endl;

	size_t h = image.height();
	size_t w = image.width();

	auto n_down = -glm::normalize(up);
	auto n_view = glm::normalize(view);
	auto n_right = glm::normalize(glm::cross(view, up));

	// from center of image plane to left of image plane
	auto img_right = n_right * tan(degreesToRadians(fovy) / 2);
	auto img_down = n_down * tan(degreesToRadians(fovy) / 2); // todo: replace this with fovx

	auto topleft = n_view - img_down - img_right;

	for (uint y = 0; y < h; ++y) {
		cout << fixed << setprecision(2) << 100.0*y/h << "% done" << "\r" << flush;
		for (uint x = 0; x < w; ++x) {

			vec3 colour = sample(0, w, h, x, y, eye, topleft, img_right, img_down, root, lights, ambient);

			image(x, y, 0) = colour.x;
			image(x, y, 1) = colour.y;
			image(x, y, 2) = colour.z;
		}
	}
		if (std::find(args.begin(), args.end(), "--ss") != args.end()) {
		cout << "supersampling..." << endl;

		Image edge_info(w, h);
		for (uint y=1; y<h-1; y++) {
			for (uint x=1;x<h-1;x++) {
				float d = image.divergence(x,y);
				bool do_supersample = d > SUPERSAMPLE_THRESHOLD;
				float intensity = do_supersample ? 1.0 : 0.0;
				edge_info(x, y, 0) = intensity;
				edge_info(x, y, 1) = intensity;
				edge_info(x, y, 2) = intensity;
			}
		}
		edge_info.savePng("edges.png");

		for (uint y=1; y<h-1; y++) {
			cout << fixed << setprecision(2) << 100.0*y/h << "% done" << "\r" << flush;
			for (uint x=1;x<h-1;x++) {
				float d = image.divergence(x,y);
				bool do_supersample = d > SUPERSAMPLE_THRESHOLD;
				if (do_supersample) {
					vec3 colour = sample(1, w, h, x, y, eye, topleft, img_right, img_down, root, lights, ambient);
					image(x, y, 0) = colour.x;
					image(x, y, 1) = colour.y;
					image(x, y, 2) = colour.z;
				}
			}
		}

	}

	cout << "num_traces=" << num_traces << endl;



}
