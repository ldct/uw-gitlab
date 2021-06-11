// Winter 2020

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include <stdio.h>

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "cs488-framework/ShaderException.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

enum raster_mode {
	PICK_MODE = 0,
	OVERLAY_MODE = 1,
	DRAW_MODE = 2
};

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
	  m_use_raytrace(false),
	  m_merge_enabled(true),
	  m_undo_warning(false),
	  m_redo_warning(false),
	  m_opt_zbuf(true),
	  m_mode(GLFW_KEY_P) {

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.85, 0.85, 0.85, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("fullscreen.obj"),
			getAssetFilePath("cube.obj"),
			getAssetFilePath("cylinder.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

JointNode* get_associated_joint(std::vector<JointNode*>& joints, int node_id) {
	for (const auto joint : joints) {
		if (joint->selection && joint->selection->m_nodeId == node_id) {
			return joint;
		}
	}
	return nullptr;
}

// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.

	auto rootNode = import_lua(filename);

	rootNode->collect_joints(m_joints);

	cout << "found " << m_joints.size() << " joints" << endl;

	m_rootNode = std::shared_ptr<SceneNode>(new SceneNode("extrusion"));
	m_rootNode->trans = rootNode->trans;
	rootNode->trans = mat4();

	m_rootNode->add_child(rootNode);

	m_initial_rootNode_trans = m_rootNode->trans;

	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}

	saveJointsState();

}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_raymarch_shader.generateProgramObject();
	m_raymarch_shader.attachVertexShader( getAssetFilePath("raytrace.vs").c_str() );
	m_raymarch_shader.attachFragmentShader( getAssetFilePath("raytrace.fs").c_str() );
	m_raymarch_shader.link();

	m_raster_shader.generateProgramObject();
	m_raster_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_raster_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	m_raster_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		auto& shader = m_use_raytrace ? m_raymarch_shader : m_raster_shader;
		m_positionAttribLocation = shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		m_normalAttribLocation = m_raster_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix() {
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	// todo: 53.0 is experimentally determined...
	m_perpsective = glm::perspective(degreesToRadians(53.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt
		(vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-5.0f, -5.0f, 0.0f);
	m_light.rgbIntensity = vec3(1.0f); // light
}

// called per frame
void A3::uploadCommonSceneUniforms() {

	auto& shader = m_use_raytrace ? m_raymarch_shader : m_raster_shader;

	shader.enable();

	if (m_use_raytrace) {
		glUniform2f(m_raymarch_shader.getUniformLocation("iResolution"), float(m_windowWidth), float(m_windowHeight));
		glUniform1i(m_raymarch_shader.getUniformLocation("merge_enabled"), m_merge_enabled);
	} else {
		glUniformMatrix4fv(shader.getUniformLocation("Perspective"), 1, GL_FALSE, value_ptr(m_perpsective));
		glUniform3fv(shader.getUniformLocation("light.position"), 1, value_ptr(m_light.position));
		glUniform3fv(shader.getUniformLocation("light.rgbIntensity"), 1, value_ptr(m_light.rgbIntensity));
		glUniform3fv(shader.getUniformLocation("ambientIntensity"), 1, value_ptr(vec3(0.2f)));
	}

	CHECK_GL_ERRORS;

	shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

void A3::resetOrientation() {
	SceneNode* s = m_rootNode->children.front();
	s->trans = mat4();
}

void A3::resetPosition() {
	m_rootNode->trans = m_initial_rootNode_trans;
}

void A3::resetJoints() {
	for (const auto& joint : m_joints) {
		joint->reset();
	}
	m_undo_stack.clear();
	m_redo_stack.clear();
	saveJointsState();
}

void applyJointsState(vector<pair<float,float>>& joint_state, vector<JointNode*>& joints) {
	if (joint_state.size() != joints.size()) {
		cout << "assert failure" << endl;
	}
	for (int i=0; i<joint_state.size(); i++) {
		joints[i]->set_joint_x(joint_state[i].first);
		joints[i]->set_joint_y(joint_state[i].second);
	}
}

void A3::undo() {
	if (m_undo_warning || m_redo_warning) return;
	if (m_undo_stack.size() <= 1) {
		m_undo_warning = true;
		return;
	}
	auto tos = m_undo_stack.back();
	m_undo_stack.pop_back();
	auto js = m_undo_stack.back();
	applyJointsState(js, m_joints);
	m_redo_stack.push_back(tos);
}

void A3::redo() {
	if (m_undo_warning || m_redo_warning) return;
	if (m_redo_stack.size() == 0) {
		m_redo_warning = true;
		return;
	}
	auto js = m_redo_stack.back();
	m_redo_stack.pop_back();
	applyJointsState(js, m_joints);
	m_undo_stack.push_back(js);
}

void A3::saveJointsState() {
	vector<pair<float,float>> joints_state;
	for (const auto& joint : m_joints) {
		joints_state.push_back(make_pair(joint->curr_joint_x, joint->curr_joint_y));
	}

	if (m_undo_stack.size() && m_undo_stack.back() == joints_state) {
	} else {
		m_undo_stack.push_back(joints_state);
		m_redo_stack.clear();
	}
}

void A3::resetAll() {
	resetPosition();
	resetOrientation();
	resetJoints();
}

void A3::guiLogic() {
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);

		if (m_undo_warning)
			ImGui::OpenPopup("noundo");

		if (ImGui::BeginPopupModal("noundo")) {
			ImGui::Text("cannot undo");
			if (ImGui::Button("ok")) {
				ImGui::CloseCurrentPopup();
				m_redo_warning = false;
				m_undo_warning = false;
			}
			ImGui::EndPopup();
		}

		if (m_redo_warning)
			ImGui::OpenPopup("noredo");

		if (ImGui::BeginPopupModal("noredo")) {
			ImGui::Text("cannot redo");
			if (ImGui::Button("ok")) {
				ImGui::CloseCurrentPopup();
				m_undo_warning = false;
				m_redo_warning = false;
			}
			ImGui::EndPopup();
		}

		if(ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Application")) {
			if (ImGui::MenuItem("Reset Position (I)")) { resetPosition(); }
			if (ImGui::MenuItem("Reset Orientation (O)")) { resetOrientation(); }
			if (ImGui::MenuItem("Reset Joints (S)")) { resetJoints(); }
			if (ImGui::MenuItem("Reset All (A)")) { resetAll(); }
			if (ImGui::MenuItem("Quit (Q)")) { glfwSetWindowShouldClose(m_window, GL_TRUE); }

            ImGui::EndMenu();
        }
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo (U)")) { undo(); }
			if (ImGui::MenuItem("Redo (R)")) { redo(); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options")) {
			ImGui::Checkbox("Circle (C)", &m_opt_circle);
			ImGui::Checkbox("Z-buffer (Z)", &m_opt_zbuf);
			ImGui::Checkbox("Backface culling (B)", &m_opt_backcull);
			ImGui::Checkbox("Frontface culling (F)", &m_opt_frontcull);

			ImGui::Checkbox("Raymarch", &m_use_raytrace);
			ImGui::Checkbox("Merge", &m_merge_enabled);

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
		} else {
			cout << "cant see menu" << endl;
		}

		ImGui::RadioButton("Position/Orientation", &m_mode, GLFW_KEY_P);
		ImGui::RadioButton("Joints", &m_mode, GLFW_KEY_J);

		ImGui::Text("%zu joints selected", m_selected.size());
		ImGui::Text("%zu undos available", m_undo_stack.size()-1);
		ImGui::Text("%zu redos available", m_redo_stack.size());

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	glEnable(GL_CULL_FACE);

	if (m_opt_zbuf) { glEnable(GL_DEPTH_TEST); } else { glDisable(GL_DEPTH_TEST); }
	if (m_opt_frontcull && m_opt_backcull) {
		glCullFace(GL_FRONT_AND_BACK);
	} else if (m_opt_frontcull) {
		glCullFace(GL_FRONT);
	} else if (m_opt_backcull) {
		glCullFace(GL_BACK);
	} else {
		glDisable(GL_CULL_FACE);
	}

	renderSceneGraph(*m_rootNode);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	if (m_opt_circle) {
		renderArcCircle();
	}

	renderSelectionOverlay();
}

void A3::renderSelectionOverlay() {

	m_raster_shader.enable();
	glUniform1i(m_raster_shader.getUniformLocation("mode"), OVERLAY_MODE);

	glBindVertexArray(m_vao_meshData);
	m_trans_stack.push(glm::mat4());
	m_rootNode->render(m_trans_stack, m_view, m_raster_shader, m_batchInfoMap, m_selected, OVERLAY_MODE);

	m_raster_shader.disable();

	CHECK_GL_ERRORS;

}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode& root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	if (m_use_raytrace) {
		glDisable(GL_CULL_FACE);
		BatchInfo batchInfo = m_batchInfoMap["fullscreen"];

		m_raymarch_shader.enable();

		m_trans_stack.push(glm::mat4());
		root.render_shading(m_trans_stack, m_view, m_raymarch_shader);

		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);

		m_raymarch_shader.disable();
	} else {
		m_raster_shader.enable();
		glUniform1i(m_raster_shader.getUniformLocation("mode"), DRAW_MODE);
		m_trans_stack.push(glm::mat4());
		root.render(m_trans_stack, m_view, m_raster_shader, m_batchInfoMap, m_selected, DRAW_MODE);
		m_raster_shader.disable();
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

float getDragX(int mouseNum) {
	auto d = ImGui::GetMouseDragDelta(mouseNum, 0.0f);
	ImGui::ResetMouseDragDelta(mouseNum);
	return std::min(d.x, 10.0f);
}

float getDragY(int mouseNum) {
	auto d = ImGui::GetMouseDragDelta(mouseNum, 0.0f);
	ImGui::ResetMouseDragDelta(mouseNum);
	return std::min(d.y, 10.0f);
}

// direct translation of trackball.c
glm::vec3 vCalcRotVec(
      float fNewX, float fNewY,
      float fOldX, float fOldY,
      float fDiameter
) {


   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,
         fOldVecX, fOldVecY, fOldVecZ,
         fLength;
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);

   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   return glm::cross(
	   glm::vec3(fOldVecX, fOldVecY, fOldVecZ),
	   glm::vec3(fNewVecX, fNewVecY, fNewVecZ)
   );
}

void A3::handleDrag(int mouseNum) {
	if (!ImGui::IsMouseDown(mouseNum) || ImGui::IsMouseHoveringAnyWindow()) {
		return;
	}
	if (m_mode == GLFW_KEY_J) {
		if (mouseNum == 2 /* middle */) {

			const float diff = getDragY(mouseNum);
			for (const auto selected_id : m_selected) {
				const auto joint = get_associated_joint(m_joints, selected_id);
				joint->set_joint_x(joint->curr_joint_x + diff);
			}

		} else if (mouseNum == 1 /* right */) {

			const float diff = getDragX(mouseNum);
			for (const auto selected_id : m_selected) {
				const auto joint = get_associated_joint(m_joints, selected_id);
				joint->set_joint_y(joint->curr_joint_y + diff);
			}
		}
	} else if (m_mode == GLFW_KEY_P) {
		if (mouseNum == 0 /* left */) {
			double xDiff = ImGui::GetMouseDragDelta(mouseNum, 0.0f).x;
			double yDiff = ImGui::GetMouseDragDelta(mouseNum, 0.0f).y;
			ImGui::ResetMouseDragDelta(mouseNum);

			xDiff /= 100;
			yDiff /= 100;

			m_rootNode->translate(vec3(xDiff, -yDiff, 0.0));
		} else if (mouseNum == 2 /*middle*/) {
			double yDiff = ImGui::GetMouseDragDelta(mouseNum, 0.0f).y;
			ImGui::ResetMouseDragDelta(mouseNum);
			yDiff /= 100;

			m_rootNode->translate(vec3(0.0,0.0,yDiff));

		} else if (mouseNum == 1 /* right */) {

			double fNewX, fNewY;
			glfwGetCursorPos( m_window, &fNewX, &fNewY );

			double fOldX = fNewX - ImGui::GetMouseDragDelta(mouseNum, 0.0f).x;
			double fOldY = fNewY - ImGui::GetMouseDragDelta(mouseNum, 0.0f).y;
			ImGui::ResetMouseDragDelta(mouseNum);

			float fDiameter;
			int iCenterX, iCenterY;
			float fNewModX, fNewModY, fOldModX, fOldModY;

			fDiameter = (m_framebufferWidth < m_framebufferHeight) ? m_framebufferWidth * 0.5 : m_framebufferHeight * 0.5;
			iCenterX = m_framebufferWidth / 2;
			iCenterY = m_framebufferHeight / 2;
			fOldModX = fOldX - iCenterX;
			fOldModY = fOldY - iCenterY;
			fNewModX = fNewX - iCenterX;
			fNewModY = fNewY - iCenterY;

	        auto fRotVec = vCalcRotVec(fNewModX, fNewModY, fOldModX, fOldModY,fDiameter);

			auto angle = glm::length(fRotVec);

			if (angle > 0) {
				fRotVec = glm::normalize(fRotVec);
				fRotVec.x = -fRotVec.x;
				fRotVec.z = -fRotVec.z;
				mat4 rot_matrix = glm::rotate(angle, fRotVec);
				SceneNode* s = m_rootNode->children.front();
				s->trans = rot_matrix * s->trans;
			}



		}
	}
}

bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	handleDrag(0); // left mouse
	handleDrag(2); // middle mouse
	handleDrag(1); // right mouse
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {

		double xpos, ypos;
		glfwGetCursorPos( m_window, &xpos, &ypos );
		xpos *= double(m_framebufferWidth) / double(m_windowWidth);
		ypos = m_windowHeight - ypos;
		ypos *= double(m_framebufferHeight) / double(m_windowHeight);
		GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };

		m_raster_shader.enable();
		glUniform1i(m_raster_shader.getUniformLocation("mode"), PICK_MODE);
		m_trans_stack.push(glm::mat4());
		m_rootNode->render(m_trans_stack, m_view, m_raster_shader, m_batchInfoMap, m_selected, PICK_MODE);
		m_raster_shader.disable();
		glReadBuffer( GL_BACK );
		glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
		CHECK_GL_ERRORS;

		unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

		if (what < 10000 /* bg */ && get_associated_joint(m_joints, what)) {
			if (m_selected.count(what)) {
				m_selected.erase(what);
			} else {
				m_selected.insert(what);
			}
		} else {
		}
	} else if (m_mode == GLFW_KEY_J && actions == GLFW_RELEASE && m_selected.size()) {
		saveJointsState();
	}


	return true;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	if (action == GLFW_PRESS && key == GLFW_KEY_M ) { show_gui = !show_gui;
	} else if (action == GLFW_PRESS && key == GLFW_KEY_C ) { m_opt_circle = !m_opt_circle;
	} else if (action == GLFW_PRESS && key == GLFW_KEY_Z ) { m_opt_zbuf = !m_opt_zbuf;
	} else if (action == GLFW_PRESS && key == GLFW_KEY_B ) { m_opt_backcull = !m_opt_backcull;
	} else if (action == GLFW_PRESS && key == GLFW_KEY_F ) { m_opt_frontcull = !m_opt_frontcull;
	} else if (key == GLFW_KEY_Q) { glfwSetWindowShouldClose(m_window, GL_TRUE);
	} else if (key == GLFW_KEY_O) { resetOrientation();
	} else if (key == GLFW_KEY_S) { resetJoints();
	} else if (key == GLFW_KEY_I) { resetPosition();
	} else if (key == GLFW_KEY_A) { resetAll();
	} else if (action == GLFW_PRESS && key == GLFW_KEY_U) { undo();
	} else if (action == GLFW_PRESS && key == GLFW_KEY_R) { redo();
	} else if (
			key == GLFW_KEY_P ||
			key == GLFW_KEY_J
	) {
		if (action == GLFW_PRESS) {
			m_mode = key;
		}
	} else {
		return false;
	}
	return true;
}
