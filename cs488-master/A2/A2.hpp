// Winter 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// 3D segment - represented by 4 numbers in homogenous coordinates
using segment3 = std::tuple<glm::vec4, glm::vec4>;

// 2D segment - 2 numbers in non-homogenous coordinates
using segment2 = std::tuple<glm::vec2, glm::vec2>;

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 100000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
		const glm::vec2 & v0,
		const glm::vec2 & v1
	);

	void myDrawLine (
		const glm::vec2 & v0,
		const glm::vec2 & v1
	);

	void drawSegment(
		const segment3& s
	);

	void handleDrag(int mouseNum, glm::vec3 axis);
	void reset();

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	std::vector<segment3> cube_segments;
	std::vector<segment3> cube_gonom;
	std::vector<segment3> world_gonom;

	glm::mat4 m_view_tr;

	glm::mat4 m_model_tr; // transform
	glm::mat4 m_model_s; // scale
	glm::mat4 m_model_r; // rotation; for shading only

	int m_mode;

	float m_near;
	float m_far;
	float m_fov;

	glm::vec2 m_viewport_a;
	glm::vec2 m_viewport_b;

	bool m_many_cubes;
};
