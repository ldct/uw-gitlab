// Winter 2020

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

#include <deque>

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void populateGlBuffer();

	glm::vec3 make_point(std::tuple<int,int>val, bool x, bool y, bool z);

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.
	GLint octaves_uni;
	GLint minimumAmbient_uni;
	GLint lightxyz_uni;
	GLint normal_uni;

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[9];
	int current_col;

	float m_camera_pitch;
	float m_rotation;
	float m_camera_dist; // in multiples of DIM

	std::unique_ptr<Maze> m_maze;

	std::vector<float> m_verts_grid;
	std::vector<float> m_verts_maze_top;
	std::vector<float> m_verts_maze_front;
	std::vector<float> m_verts_maze_back;
	std::vector<float> m_verts_maze_right;
	std::vector<float> m_verts_maze;
	std::vector<float> m_verts_avatar;
	std::vector<float> m_verts_floor;

	float m_wall_height;

	std::deque<float> m_mouse_boosts;
	float m_rotation_v;

	std::tuple<int,int> m_avatar_coordinates;
	float m_avatar_height;
	float m_avatar_velocity;
};
