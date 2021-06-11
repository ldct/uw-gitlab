// Winter 2020

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <deque>
#include <numeric>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
    : current_col( 0 ),
      m_camera_pitch(0.7),
      m_maze(new Maze(DIM)),
      m_wall_height(1.0f),
      m_rotation(0.0f),
      m_camera_dist(2.6f),
      m_rotation_v(0.0f),
      m_avatar_height(10.0f),
      m_avatar_velocity(0.0f)
{
    // initial floor
    colour[0] = 1.0f;
    colour[1] = 1.0f;
    colour[2] = 1.0f;

    // initial maze
    colour[3] = 0.0f;
    colour[4] = 0.0f;
    colour[5] = 0.0f;

    // initial avatar
    colour[6] = 0.0f;
    colour[7] = 0.0f;
    colour[8] = 0.5f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}


using triangle = tuple<glm::vec3, glm::vec3, glm::vec3>;

/*

p0
|
|
p01   p02
|
|
p1----p12----p2
*/
vector<triangle> subdivide(triangle t) {
    auto p0 = get<0>(t);
    auto p1 = get<1>(t);
    auto p2 = get<2>(t);

    auto p01 = 0.5f*(p0+p1);
    auto p02 = 0.5f*(p0+p2);
    auto p12 = 0.5f*(p1+p2);

    return vector<triangle>({
        make_tuple(p0,p01,p02),
        make_tuple(p01,p1,p12),
        make_tuple(p01,p02,p12),
        make_tuple(p02,p12,p2)
    });
}

vector<triangle> subdivide_times(vector<triangle>& ts, int n) {
    if (n == 0) return ts;
    auto ret = vector<triangle>();
    for(auto const& t: ts) {
        auto subdivided = subdivide(t);
        ret.insert(ret.end(),subdivided.begin(),subdivided.end());
    }
    return subdivide_times(ret, n-1);
}

vector<triangle> make_tetrahedron() {
    float SQRT2INV = 1.0f/sqrt(2.0f);
    vec3 p0 = vec3(1.0f,0.0f,-SQRT2INV);
    vec3 p1 = vec3(-1.0f,0.0f,-SQRT2INV);
    vec3 p2 = vec3(0.0f,1.0f,SQRT2INV);
    vec3 p3 = vec3(0.0f,-1.0f,SQRT2INV);

    return vector<triangle>({
        make_tuple(p1, p2, p3),
        make_tuple(p0, p2, p3),
        make_tuple(p0, p1, p3),
        make_tuple(p0, p1, p2)
    });
}

vector<triangle> make_sphere_t() {
    auto tetrahedron = make_tetrahedron();
    auto s = subdivide_times(tetrahedron, 4);
    for (int i=0; i<s.size(); i++) {
        s[i] = make_tuple(
            0.5f * glm::normalize(get<0>(s[i])),
            0.5f * glm::normalize(get<1>(s[i])),
            0.5f * glm::normalize(get<2>(s[i]))
        );
    }
    return s;
}

vector<float> make_sphere() {
    vector<float> ret = vector<float>();
    auto sphere = make_sphere_t();
    for (auto const& t: sphere) {
        vector<vec3> tt = vector<vec3>({get<0>(t), get<1>(t), get<2>(t)});
        for (auto const& p: tt) {
            ret.push_back(p.x);
            ret.push_back(p.y);
            ret.push_back(p.z);
        }
    }
    return ret;
}

void push_point(vector<float>& verts, glm::vec3 pt) {
    verts.push_back(pt.x);
    verts.push_back(pt.y);
    verts.push_back(pt.z);
}

// p2 and p3 are shared
void push_face(
        vector<float>& verts,
        glm::vec3 p1,
        glm::vec3 p2,
        glm::vec3 p3,
        glm::vec3 p4
) {
    push_point(verts, p1);
    push_point(verts, p2);
    push_point(verts, p3);

    push_point(verts, p2);
    push_point(verts, p3);
    push_point(verts, p4);
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
    // Initialize random number generator
    int rseed=getpid();
    srandom(rseed);
    // Print random number seed in case we want to rerun with
    // same random numbers
    cout << "Random number seed = " << rseed << endl;

    // Set the background colour.
    glClearColor( 0.3, 0.5, 0.7, 1.0 );

    // Build the shader
    m_shader.generateProgramObject();
    m_shader.attachVertexShader(
        getAssetFilePath( "VertexShader.vs" ).c_str() );
    m_shader.attachFragmentShader(
        getAssetFilePath( "FragmentShader.fs" ).c_str() );
    m_shader.link();

    // Set up the uniforms
    P_uni = m_shader.getUniformLocation( "P" );
    V_uni = m_shader.getUniformLocation( "V" );
    M_uni = m_shader.getUniformLocation( "M" );
    col_uni = m_shader.getUniformLocation( "colour" );
    octaves_uni = m_shader.getUniformLocation( "octaves" );
    minimumAmbient_uni = m_shader.getUniformLocation("minimumAmbient");
    lightxyz_uni = m_shader.getUniformLocation("lightxyz");
    normal_uni = m_shader.getUniformLocation("normal");

    // grid, avatar and floor never change

    size_t ct = 0;
    for (int idx = 0; idx < DIM+3; ++idx) {
        // horizontal line
        m_verts_grid.push_back(-1);
        m_verts_grid.push_back(0);
        m_verts_grid.push_back(idx-1);
        m_verts_grid.push_back(DIM+1);
        m_verts_grid.push_back(0);
        m_verts_grid.push_back(idx-1);

        // vertical line
        m_verts_grid.push_back(idx-1);
        m_verts_grid.push_back(0);
        m_verts_grid.push_back(-1);
        m_verts_grid.push_back(idx-1);
        m_verts_grid.push_back(0);
        m_verts_grid.push_back(DIM+1);
    }

    auto sphere = make_sphere();
    for (auto const& f: sphere) {
        m_verts_avatar.push_back(f);
    }

    auto p0 = glm::vec3(0.0f, -0.01f, 0.0f);
    auto p1 = glm::vec3(0.0f, -0.01f, float(DIM));
    auto p2 = glm::vec3(float(DIM), -0.01f, 0.0f);
    auto p3 = glm::vec3(float(DIM), -0.01f, float(DIM));

    push_face(m_verts_floor, p0, p1, p2, p3);

    populateGlBuffer();

    proj = glm::perspective(
        glm::radians( 90.0f ),
        1.0f,
        1.0f, 1000.0f );

    cout << "proj = " << proj << endl;
}

glm::vec3 A1::make_point(tuple<int,int>val, bool x, bool y, bool z) {
    return glm::vec3(
        get<0>(val) + x,
        y ? m_wall_height : 0.0f,
        get<1>(val) + z
    );
}

void A1::populateGlBuffer()
{
    m_verts_maze_top.clear();
    m_verts_maze_front.clear();
    m_verts_maze_back.clear();
    m_verts_maze_right.clear();
    m_verts_maze.clear();

    for(auto const& val: m_maze->blocks()) {

        // constant y => top and bottom faces

        // top face
        push_face(
            m_verts_maze_top,
            make_point(val, 0, 1, 0),
            make_point(val, 1, 1, 0),
            make_point(val, 0, 1, 1),
            make_point(val, 1, 1, 1)
        );

        // constant z => front and back faces

        // back face
        if (!m_maze->getValue(get<0>(val), get<1>(val)-1))
            push_face(
                m_verts_maze_back,
                make_point(val, 0, 0, 0),
                make_point(val, 0, 1, 0),
                make_point(val, 1, 0, 0),
                make_point(val, 1, 1, 0)
            );
        // front face
        if (!m_maze->getValue(get<0>(val), get<1>(val)+1))
            push_face(
                m_verts_maze_front,
                make_point(val, 0, 0, 1),
                make_point(val, 0, 1, 1),
                make_point(val, 1, 0, 1),
                make_point(val, 1, 1, 1)
            );

        // constant x => side faces

        // left face
        if (!m_maze->getValue(get<0>(val)-1, get<1>(val)))
            push_face(
                m_verts_maze,
                make_point(val, 0, 0, 0),
                make_point(val, 0, 0, 1),
                make_point(val, 0, 1, 0),
                make_point(val, 0, 1, 1)
            );

        // right face
        if (!m_maze->getValue(get<0>(val)+1, get<1>(val)))
            push_face(
                m_verts_maze_right,
                make_point(val, 1, 0, 0),
                make_point(val, 1, 0, 1),
                make_point(val, 1, 1, 0),
                make_point(val, 1, 1, 1)
            );
    }

    vector<float> m_verts;
    m_verts.insert(m_verts.end(), m_verts_grid.begin(), m_verts_grid.end());
    m_verts.insert(m_verts.end(), m_verts_floor.begin(), m_verts_floor.end());
    m_verts.insert(m_verts.end(), m_verts_maze_top.begin(), m_verts_maze_top.end());
    m_verts.insert(m_verts.end(), m_verts_maze_front.begin(), m_verts_maze_front.end());
    m_verts.insert(m_verts.end(), m_verts_maze_back.begin(), m_verts_maze_back.end());
    m_verts.insert(m_verts.end(), m_verts_maze_right.begin(), m_verts_maze_right.end());
    m_verts.insert(m_verts.end(), m_verts_maze.begin(), m_verts_maze.end());
    m_verts.insert(m_verts.end(), m_verts_avatar.begin(), m_verts_avatar.end());

    // Create the vertex array to record buffer assignments.
    glGenVertexArrays( 1, &m_grid_vao );
    glBindVertexArray( m_grid_vao );

    // Create the cube vertex buffer
    glGenBuffers( 1, &m_grid_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
    glBufferData( GL_ARRAY_BUFFER, m_verts.size()*sizeof(float),
        m_verts.data(), GL_STATIC_DRAW );

    // Specify the means of extracting the position values properly.
    GLint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{

    if (m_avatar_height > 0.52f) {
        m_avatar_velocity -= 0.015f;
    }
    if (m_avatar_height < 0.48f) {
        m_avatar_height = 0.5f;
        m_avatar_velocity = -0.8*m_avatar_velocity;
    }
    if (fabs(m_avatar_height-0.5) < 0.1 && fabs(m_avatar_velocity) < 0.01) {
        m_avatar_height = 0.5;
        m_avatar_velocity = 0.0;
    }
    m_avatar_height += m_avatar_velocity;

    if (ImGui::IsMouseDown(0) && !ImGui::IsMouseHoveringAnyWindow()) {
        auto d = ImGui::GetMouseDragDelta(0, 0.0f);

        d.x = std::min(d.x, 10.0f);

        m_rotation += d.x / 100.0f;
        m_mouse_boosts.push_front(d.x);
        if (m_mouse_boosts.size() > 10) {
            m_mouse_boosts.pop_back();
        }
        ImGui::ResetMouseDragDelta(0);
    } else {
        m_rotation += m_rotation_v / 100.0f;
    }

    if (ImGui::IsMouseDown(1)) {
        auto d = ImGui::GetMouseDragDelta(1, 0.0f);
        m_camera_pitch -= d.y / 100.0f;
        ImGui::ResetMouseDragDelta(1);
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
    // We already know there's only going to be one window, so for
    // simplicity we'll store button states in static local variables.
    // If there was ever a possibility of having multiple instances of
    // A1 running simultaneously, this would break; you'd want to make
    // this into instance fields of A1.
    static bool showTestWindow(false);
    static bool showDebugWindow(true);

    ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
    float opacity(0.5f);

    ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
        if( ImGui::Button( "Quit Application" ) ) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }

        ImGui::SliderFloat("rotation", &m_rotation, -1.5f, 1.5f);
        ImGui::SliderFloat("pitch", &m_camera_pitch, 0.1f, 1.7f);

        // Eventually you'll create multiple colour widgets with
        // radio buttons.  If you use PushID/PopID to give them all
        // unique IDs, then ImGui will be able to keep them separate.
        // This is unnecessary with a single colour selector and
        // radio button, but I'm leaving it in as an example.

        // Prefixing a widget name with "##" keeps it from being
        // displayed.

        ImGui::ColorEdit3( "Colour", &colour[3*current_col] );
        if( ImGui::RadioButton( "Floor", &current_col, 0 ) ) {}
        ImGui::SameLine();
        if( ImGui::RadioButton( "Maze", &current_col, 1 ) ) {}
        ImGui::SameLine();
        if( ImGui::RadioButton( "Avatar", &current_col, 2 ) ) {}

/*
        // For convenience, you can uncomment this to show ImGui's massive
        // demonstration window right in your application.  Very handy for
        // browsing around to get the widget you want.  Then look in
        // shared/imgui/imgui_demo.cpp to see how it's done.
        if( ImGui::Button( "Test Window" ) ) {
            showTestWindow = !showTestWindow;
        }
*/

        ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

    ImGui::End();

    if( showTestWindow ) {
        ImGui::ShowTestWindow( &showTestWindow );
    }
}

void doDraw(GLenum mode, int& m_verts_idx, vector<float>& verts, bool visible) {
    if (visible) {
        glDrawArrays(mode, m_verts_idx, verts.size() / 3 );
    }
    m_verts_idx += verts.size() / 3;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
    // Create a global transformation for the model (centre it).
    mat4 W;
    W = glm::rotate( W, -m_rotation, glm::vec3(0.0f, 1.0f, 0.0f) );
    W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

    m_shader.enable();
        glEnable( GL_DEPTH_TEST );

        auto eye = glm::vec3( 0.0f, m_camera_dist*float(DIM)*cos(m_camera_pitch), m_camera_dist*float(DIM)*sin(m_camera_pitch) );
        glm::vec3 orth = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::cross(eye, orth);
        // todo: camera up should transform according to camera_pitch
        view = glm::lookAt(
            eye,
            glm::vec3( 0.0f, 0.0f, 0.0f ),
            up
        );

        glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
        glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
        glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

        glUniform3f(lightxyz_uni, get<0>(m_avatar_coordinates)+0.5f, m_avatar_height, get<1>(m_avatar_coordinates)+0.5f);

        int m_verts_idx = 0;

        // grid + floor.
        glBindVertexArray( m_grid_vao );
        glUniform1i( octaves_uni, 1 );
        glUniform1f( minimumAmbient_uni, 0.2f);
        glUniform3f( col_uni, colour[0], colour[1], colour[2] );
        glUniform3f( normal_uni, 0.0f, 1.0, 0.0f);
        doDraw(GL_LINES, m_verts_idx, m_verts_grid, true);
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_floor, true);

        glUniform1i( octaves_uni, 6 );

        // maze
        glUniform3f( col_uni, colour[3], colour[4], colour[5] );
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_maze_top, true);
        glUniform3f( normal_uni, 0.0f, 0.0f, 1.0f);
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_maze_front, true);
        glUniform3f( normal_uni, 0.0f, 0.0f, -1.0f);
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_maze_back, true);
        glUniform3f( normal_uni, 1.0f, 0.0f, 0.0f);
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_maze_right, true);
        glUniform3f( normal_uni, -1.0f, 0.0f, 0.0f);
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_maze, true);

        // avatar
        W = glm::translate( W, vec3( 0.5f + get<0>(m_avatar_coordinates), m_avatar_height, 0.5f + get<1>(m_avatar_coordinates) ) );
        glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );
        glUniform1f( minimumAmbient_uni, 1.0f);
        glUniform3f( col_uni, colour[6], colour[7], colour[8] );
        doDraw(GL_TRIANGLES, m_verts_idx, m_verts_avatar, true);


    m_shader.disable();

    // Restore defaults
    glBindVertexArray( 0 );

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
        int entered
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos)
{
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // Put some code here to handle rotations.  Probably need to
        // check whether we're *dragging*, not just moving the mouse.
        // Probably need some instance variables to track the current
        // rotation amount, and maybe the previous X position (so
        // that you can rotate relative to the *change* in X.
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
    bool eventHandled(false);

    if (ImGui::IsMouseDown(0)) {
        auto b = accumulate(m_mouse_boosts.begin(), m_mouse_boosts.end(), 0);
        m_rotation_v = b/10.0f;
    } else {
        m_mouse_boosts.clear();
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
    bool eventHandled(false);

    if (yOffSet > 0) {
        m_camera_dist -= 0.1f;
    } else if (yOffSet < 0) {
        m_camera_dist += 0.1f;
    }

    // Zoom in or out.

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
    if(action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            m_camera_pitch = 0.7;
            m_wall_height = 1.0f;
            m_rotation = 0.0f;
            m_camera_dist = 2.6f;
            m_rotation_v = 0.0f;
            m_maze->reset();
            colour[0] = 1.0f;
            colour[1] = 1.0f;
            colour[2] = 1.0f;
            colour[3] = 0.0f;
            colour[4] = 0.0f;
            colour[5] = 0.0f;
            colour[6] = 0.0f;
            colour[7] = 0.0f;
            colour[8] = 0.5f;
            populateGlBuffer();
        } else if (key == GLFW_KEY_D) {
            m_maze->digMaze();
            m_avatar_coordinates = make_tuple(0, m_maze->start);
            populateGlBuffer();
        } else if (key == GLFW_KEY_BACKSPACE) {
            if (m_wall_height > 1.0f) {
                m_wall_height -= 1.0f;
            }
            populateGlBuffer();
        } else if (key == GLFW_KEY_SPACE) {
            m_wall_height += 1.0f;
            populateGlBuffer();
        } else if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
        } else if (
                (key == GLFW_KEY_LEFT) ||
                (key == GLFW_KEY_RIGHT) ||
                (key == GLFW_KEY_UP) ||
                (key == GLFW_KEY_DOWN)
        ) {
            std::tuple<int,int> new_mac = m_avatar_coordinates;

            if (key == GLFW_KEY_LEFT) {
                get<0>(new_mac) -= 1;
            } else if (key == GLFW_KEY_RIGHT) {
                get<0>(new_mac) += 1;
            } else if (key == GLFW_KEY_UP) {
                get<1>(new_mac) -= 1;
            } else if (key == GLFW_KEY_DOWN) {
                get<1>(new_mac) += 1;
            }

            if (mods & GLFW_MOD_SHIFT) {
                m_maze->setValue(get<0>(new_mac), get<1>(new_mac), 0);
                populateGlBuffer();
            }

            if (!m_maze->getValue(get<0>(new_mac), get<1>(new_mac))) {
                m_avatar_coordinates = new_mac;
            }
        } else if (key == GLFW_KEY_J) {
            m_avatar_velocity = 0.3f;
        } else if (
            (key == GLFW_KEY_LEFT_SHIFT) ||
            (key == GLFW_KEY_RIGHT_SHIFT) ||
            (key == GLFW_KEY_LEFT_ALT) ||
            (key == GLFW_KEY_RIGHT_ALT) ||
            (key == GLFW_KEY_TAB)
        ) {
            // ignore
		} else {
			cout << "unhandled: " << key << endl;
            return false;
		}
    }

    return true;
}
