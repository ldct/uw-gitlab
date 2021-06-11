// Winter 2020

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

segment3 operator*(mat4 M, segment3 p) {
	return make_tuple(
		M*get<0>(p),
		M*get<1>(p)
	);
}

vec4 augment(vec3 o) {
	return vec4(o.x, o.y, o.z, 0);
}

segment3 operator+(segment3 p, vec3 o) {
	return make_tuple(
		get<0>(p) + augment(o),
		get<1>(p) + augment(o)
	);
}

void push_segment(
	vector<segment3>& segments,
	float x1,
	float y1,
	float z1,
	float x2,
	float y2,
	float z2
) {
	segments.push_back(make_tuple(
		glm::vec4(x1, y1, z1, 1.0),
		glm::vec4(x2, y2, z2, 1.0)
	));
}

void push_spike(
	vector<segment3>& segments,
	int x, int y, int z
) {
	push_segment(segments, x, y, z, x, y, -z);
	push_segment(segments, x, y, z, x, -y, z);
	push_segment(segments, x, y, z, -x, y, z);
}

void push_gonom(
	vector<segment3>& segments
) {
	push_segment(segments, 0, 0, 0, 0, 0, 1);
	push_segment(segments, 0, 0, 0, 0, 1, 0);
	push_segment(segments, 0, 0, 0, 1, 0, 0);
}

void push_standard_cube(
	vector<segment3>& segments
) {
	push_spike(segments, -1, -1, -1);
	push_spike(segments, -1,  1,  1);
	push_spike(segments,  1, -1,  1);
	push_spike(segments,  1,  1, -1);
}

std::ostream & operator<<(std::ostream & o, segment3 & s) {
  return o << get<0>(s) << get<1>(s);
}

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}

// = glm::translate(I, direction);
mat4 translation(vec3 direction) {
	return transpose(mat4(
		1, 0, 0, direction.x,
		0, 1, 0, direction.y,
		0, 0, 1, direction.z,
		0, 0, 0, 1
	));
}

//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
	reset();
}

void A2::reset() {
	m_model_r = mat4();
	m_model_s = mat4();
	m_model_tr = mat4();
	m_mode = GLFW_KEY_R;
	m_near = 1.0;
	m_far = 100.0;
	m_fov = 90.0;
	m_viewport_a = vec2(-0.95, -0.95);
	m_viewport_b = vec2(0.95, 0.95);
	m_many_cubes = false;
	m_view_tr = translation(vec3(0, 0, -4.0));
}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init() {
	glClearColor(0.3, 0.5, 0.7, 1.0);
	createShaderProgram();
	glGenVertexArrays(1, &m_vao);
	enableVertexAttribIndices();
	generateVertexBuffers();
	mapVboDataToVertexAttributeLocation();

	push_gonom(cube_gonom);
	push_gonom(world_gonom);
	push_standard_cube(cube_segments);
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram() {
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Winter 2020
void A2::enableVertexAttribIndices() {
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers() {
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation() {
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData() {
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

vector<segment2> clip2(const segment2& s, vec2 n, vec2 p) {
	vector<segment2> ret;

	auto a = get<0>(s);
	auto b = get<1>(s);

	auto la = dot(a - p, n);
	auto lb = dot(b - p, n);

	if (la <= 0 && lb <= 0) return ret;
	if (la > 0 && lb > 0) {
		ret.push_back(s);
		return ret;
	}

	float tu = dot((a - p), n);
	float td = dot((a - b), n);

	if (td == 0 && tu == 0) {
		cout << "warn: line lies on clip plane" << endl;
		return ret;
	}

	float t = tu / td;

	auto ab = (1-t)*a + t*b;

	ret.push_back(make_tuple(ab, tu > 0 ? a : b));

	return ret;
}

vector<segment2> clipNDC(const segment2& s) {

	auto opt_clipped = clip2(s, vec2(0, 1), vec2(0, -1));
	if (opt_clipped.size() == 0) return opt_clipped;
	opt_clipped = clip2(opt_clipped[0], vec2(0, -1), vec2(0, 1));
	if (opt_clipped.size() == 0) return opt_clipped;
	opt_clipped = clip2(opt_clipped[0], vec2(1, 0), vec2(-1, 0));
	if (opt_clipped.size() == 0) return opt_clipped;
	opt_clipped = clip2(opt_clipped[0], vec2(-1, 0), vec2(1, 0));

	return opt_clipped;
}

void A2::myDrawLine(
		const glm::vec2 & V0,
		const glm::vec2 & V1
) {

	auto x_min = std::min(m_viewport_a.x, m_viewport_b.x);
	auto x_max = std::max(m_viewport_a.x, m_viewport_b.x);
	auto y_min = std::min(m_viewport_a.y, m_viewport_b.y);
	auto y_max = std::max(m_viewport_a.y, m_viewport_b.y);

	if (x_min == x_max) x_max += 0.1f;
	if (y_min == y_max) y_max += 0.1f;

	auto unclipped = make_tuple(V0, V1);
	auto clipped = clipNDC(unclipped);

	if (clipped.size() == 0) return;

	auto v0 = get<0>(clipped[0]);
	auto v1 = get<1>(clipped[0]);

	v0 = vec2(
		mix(x_min, x_max, (v0.x + 1) / 2),
		mix(y_min, y_max, (v0.y + 1) / 2)
	);
	v1 = vec2(
		mix(x_min, x_max, (v1.x + 1) / 2),
		mix(y_min, y_max, (v1.y + 1) / 2)
	);
	drawLine(v0, v1);
}

void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	if (abs(V0.x) > 1) {
		cout << "clip warning: V0.x=" << V0.x << endl;
	}

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

vec2 xy(vec4 p) {
	return vec2(p.x, p.y);
}

vec2 xy(vec3 p) {
	return vec2(p.x, p.y);
}

vec3 xyz(vec4 p) {
	return vec3(p.x, p.y, p.z);
}

vec3 xyw(vec4 p) {
	return vec3(p.x, p.y, p.w);
}

vec3 normalized_hom(vec3 p) {
	return vec3(p.x / p.z, p.y / p.z, 1);
}

vector<segment3> clip(const segment3& s, vec4 n, vec4 p) {
	vector<segment3> ret;

	auto a = get<0>(s);
	auto b = get<1>(s);

	auto la = dot(a - p, n);
	auto lb = dot(b - p, n);

	if (la < 0 && lb < 0) return ret;
	if (la > 0 && lb > 0) {
		ret.push_back(s);
		return ret;
	}

	float tu = dot((a - p), n);
	float td = dot((a - b), n);
	float t = tu / td;

	auto ab = (1-t)*a + t*b;

	ret.push_back(make_tuple(ab, tu > 0 ? a : b));

	return ret;
}

// = glm::rotate(I, t, axis)
mat4 x_rotation(float t, vec3 axis) {
	auto rx = mat3(
		1, 0, 0,
		0, cos(t), -sin(t),
		0, sin(t), cos(t)
	);
	auto ry = mat3(
		cos(t), 0, sin(t),
		0, 1, 0,
		-sin(t), 0, cos(t)
	);
	auto rz = mat3(
		cos(t), -sin(t), 0,
		sin(t), cos(t), 0,
		0, 0, 1
	);
	return transpose(mat4(
		axis.x*rx + axis.y*ry + axis.z*rz
	));
}

float cot(float t) {
	return 1.0f/tan(t);
}

void A2::drawSegment(const segment3& s) {

	// near clipping

	auto opt_s = clip(s, vec4(0, 0, -1, 0), vec4(0, 0, -m_near, 0));
	if (opt_s.size() == 0) return;

	// far clipping

	opt_s = clip(opt_s[0], vec4(0, 0, 1, 0), vec4(0, 0, -m_far, 0));
	if (opt_s.size() == 0) return;

	auto t = glm::radians( m_fov ) / 2;
	auto proj2 = glm::transpose(glm::mat4(
		cot(t), 0, 0, 0,
		0, cot(t), 0, 0,
		0, 0, 0, 0,
		0, 0, -1, 0
	));

	auto p = proj2*opt_s[0];

	auto t0 = normalized_hom(xyw(get<0>(p)));
	auto t1 = normalized_hom(xyw(get<1>(p)));

	myDrawLine(xy(t0), xy(t1));
}

float z_frac_of(const segment3& s, bool is_gonom) {
	auto z_sum = get<0>(s).z + get<1>(s).z + 3;
	if (is_gonom) {
		z_sum = 2*get<1>(s).z + 3;
	}
	float z_frac = 1/z_sum;
	z_frac = clamp(z_frac, 0.0f, 0.8f);
}

void A2::appLogic() {

	mat4 V = m_view_tr;

	mat4 M = m_model_tr;
	mat4 S = m_model_s;
	mat4 R = m_model_r;

	// Call at the beginning of frame, before drawing lines:
	initLineData();

	const vec3 bg = vec3(0.3, 0.5, 0.7);

	for (auto& segment: cube_segments) {
		float z_frac = z_frac_of(R*segment, false);
		setLineColour(mix(vec3(1.0), bg, z_frac));

		if (m_many_cubes) {
			for (int i=-2; i<=2; i++) {
				for (int j=-2; j<=2; j++) {
					drawSegment(V*(M*S*segment + vec3(
						i*3, j*3, 0
					)));
				}
			}
		} else {
			drawSegment(V*(M*S*segment));
		}
	}

	for (auto& segment: world_gonom) {
		setLineColour(xyz(get<1>(segment)));
		drawSegment(V*segment);
	}

	for (auto& segment: cube_gonom) {
		float z_frac = z_frac_of(R*segment, true);
		auto colour = xyz(get<1>(segment));
		colour = vec3(1, 1, 1) - colour;

		setLineColour(mix(colour, bg, z_frac));
		drawSegment(V*M*segment);
	}

	// bounding box
	setLineColour(vec3(1, 0, 0));

	auto x_min = std::min(m_viewport_a.x, m_viewport_b.x);
	auto x_max = std::max(m_viewport_a.x, m_viewport_b.x);
	auto y_min = std::min(m_viewport_a.y, m_viewport_b.y);
	auto y_max = std::max(m_viewport_a.y, m_viewport_b.y);


	drawLine(vec2(x_min, y_min), vec2(x_min, y_max));
	drawLine(vec2(x_min, y_min), vec2(x_max, y_min));
	drawLine(vec2(x_max, y_max), vec2(x_min, y_max));
	drawLine(vec2(x_max, y_max), vec2(x_max, y_min));

}

const char* mode_name(int mode) {
	if (mode == GLFW_KEY_O) return "view rotate";
	if (mode == GLFW_KEY_E) return "view translate";
	if (mode == GLFW_KEY_P) return "perspective";
	if (mode == GLFW_KEY_R) return "model rotate";
	if (mode == GLFW_KEY_T) return "model translate";
	if (mode == GLFW_KEY_S) return "model scale";
	if (mode == GLFW_KEY_V) return "viewport";
	return "unknown";
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		ImGui::RadioButton("view rotate", &m_mode, GLFW_KEY_O);
		ImGui::RadioButton("view translate", &m_mode, GLFW_KEY_E);
		ImGui::RadioButton("perspective", &m_mode, GLFW_KEY_P);
		ImGui::RadioButton("model rotate", &m_mode, GLFW_KEY_R);
		ImGui::RadioButton("model translate", &m_mode, GLFW_KEY_T);
		ImGui::RadioButton("model scale", &m_mode, GLFW_KEY_S);
		ImGui::RadioButton("viewport", &m_mode, GLFW_KEY_V);

		if( ImGui::Button( "Quit Application" ) ) glfwSetWindowShouldClose(m_window, GL_TRUE);
		ImGui::SameLine();
		if (ImGui::Button("Reset")) reset();

		ImGui::Text("near=%.2f far=%.0f fov=%.0f", m_near, m_far, m_fov);

		auto pt = m_model_tr * vec4(0, 0, 0, 1);
		ImGui::Text("cube = %+.2f %+.2f %+.2f", pt.x, pt.y, pt.z);

		auto eye = -(m_view_tr * vec4(0, 0, 0, 1));
		ImGui::Text("eye  = %+.2f %+.2f %+.2f", eye.x, eye.y, eye.z);


        ImGui::Checkbox("many cubes", &m_many_cubes);

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

float getDrag(int mouseNum) {
	auto d = ImGui::GetMouseDragDelta(mouseNum, 0.0f);
	ImGui::ResetMouseDragDelta(mouseNum);
	return std::min(d.x, 10.0f);
}

void A2::handleDrag(int mouseNum, vec3 axis) {
	if (!ImGui::IsMouseDown(mouseNum) || ImGui::IsMouseHoveringAnyWindow()) {
		return;
	}
	if (m_mode == GLFW_KEY_O) {
		auto drag = getDrag(mouseNum);
		m_view_tr = x_rotation(-drag/70, axis)*m_view_tr;
	} else if (m_mode == GLFW_KEY_E) {
		auto drag = getDrag(mouseNum);
		m_view_tr = translation((drag/70)*axis)*m_view_tr;
	} else if (m_mode == GLFW_KEY_P) {
		if (mouseNum == 0) {
			m_fov += getDrag(mouseNum) / 3;
			m_fov = clamp(m_fov, 5.0f, 160.0f);
		} else if (mouseNum == 2) { // middle
			m_near += getDrag(mouseNum) / 10;
			m_near = clamp(m_near, 0.1f, m_far);
		} else if (mouseNum == 1) {
			m_far += getDrag(mouseNum) / 3;
			m_far = clamp(m_far, m_near, 1000.0f);
		}
	} else if (m_mode == GLFW_KEY_R) {
		auto drag = getDrag(mouseNum);
		auto r = x_rotation(-drag / 70, axis);
		m_model_tr *= r; m_model_r *= r;
	} else if (m_mode == GLFW_KEY_T) {
		auto drag = getDrag(mouseNum);
		m_model_tr *= translation((drag/70)*axis);
	} else if (m_mode == GLFW_KEY_V) {
		if (mouseNum != 0) return;

		auto a = ImGui::GetIO().MouseClickedPos[0];
		auto a_ndc = vec2(
			clamp(2*(a.x / m_windowWidth) - 1, -1.0f , 1.0f),
			clamp(1 - 2*(a.y / m_windowHeight), -1.0f, 1.0f)
		);

		auto b = ImGui::GetMousePos();
		auto b_ndc = vec2(
			clamp(2*(b.x / m_windowWidth) - 1, -1.0f, 1.0f),
			clamp(1 - 2*(b.y / m_windowHeight), -1.0f, 1.0f)
		);

		m_viewport_a = a_ndc;
		m_viewport_b = b_ndc;
	} else if (m_mode == GLFW_KEY_S) {
		auto drag = getDrag(mouseNum);

		auto scale_vec = (1+drag/70)*axis + vec3(1) - axis;
		mat4 s = glm::mat4(
			scale_vec.x, 0, 0, 0,
			0, scale_vec.y, 0, 0,
			0, 0, scale_vec.z, 0,
			0, 0, 0, 1
		);
		m_model_s *= s;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	handleDrag(0, vec3(1, 0, 0)); // left mouse
	handleDrag(2, vec3(0, 1, 0)); // middle mouse
	handleDrag(1, vec3(0, 0, 1)); // right mouse

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
 	if (key == GLFW_KEY_Q) {
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	} else if (key == GLFW_KEY_A) {
		reset();
	} else if (
			key == GLFW_KEY_O ||
			key == GLFW_KEY_E ||
			key == GLFW_KEY_P ||
			key == GLFW_KEY_R ||
			key == GLFW_KEY_T ||
			key == GLFW_KEY_S ||
			key == GLFW_KEY_V
	) {
		if (action == GLFW_PRESS) {
			m_mode = key;
		}
	} else {
		cout << "unhandled: " << key << endl;
		return false;
	}
	return true;
}
