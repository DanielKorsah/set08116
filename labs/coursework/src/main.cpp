#include <glm\glm.hpp>
#include <graphics_framework.h>

#include "meshGen.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;


geometry terrain;
geometry water;
mesh terrain_mesh;
mesh water_mesh;

effect eff;
effect sky_eff;
effect water_eff;


free_camera f_cam;
chase_camera c_cam;
target_camera t_cam;
//start using free camera
bool c1 = true;
bool c2 = false;
bool c3 = false;


//global directional light
directional_light sun_dir;
//point lights
vector<point_light> points;
//spot light
spot_light spot;

//map of meshes
map<string, mesh> meshes;

//map of textures
map<string, texture> tex;

frame_buffer frame;
geometry screen;

//transform hierarchy map
map<mesh*, mesh*> hierarchy;

//skybox cubemap
cubemap sky_cube;

mesh skybox;

double cursor_x = 0.0;
double cursor_y = 0.0;

//rotation steps
float rot_speed;

//rate of light transform
float light_speed;

shadow_map shadow;

bool initialise()
{
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_x);
	return true;
}

bool load_content() {
	
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
	//load lights
	{
		//set up the directional
		sun_dir.set_direction(normalize(vec3(-1, 0.1, 0)));
		sun_dir.set_ambient_intensity(vec4(0.1, 0.1, 0.1, 1.0));
		//set colour as white
		sun_dir.set_light_colour(vec4(0.5f, 0.5f, 0.5f, 1));
		
	}

	//load geometry meshes
	{
		vector<vec3> positions = makeMesh();
		water.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
		water_mesh.set_geometry(water);
	}

	//Load Textures
	{
		
	}


	//set materials
	{
		//reflecition material
		material mat;

		mat.set_specular(vec4(0.4f));
		mat.set_shininess(25.0f);
		mat.set_diffuse(vec4(0.2f, 0.2f, 1.0f, 1.0f));

		water_mesh.set_material(mat);
	}


	// Get and bind shaders
	{ 
		//skybox shaders
		sky_eff.add_shader("res/shaders/Skybox.vert", GL_VERTEX_SHADER);
		sky_eff.add_shader("res/shaders/Skybox.frag", GL_FRAGMENT_SHADER);

		water_eff.add_shader("res/shaders/gouraud.vert", GL_VERTEX_SHADER);



		sky_eff.build();
		water_eff.build();
		
	}


	//camera properties
	{
		// Set free camera properties
		f_cam.set_position(vec3(0, 20, -20));
		f_cam.set_projection(half_pi<float>(), renderer::get_screen_aspect(), 0.1f, 4000.0f);

		//set chase cam roperties
		c_cam.set_pos_offset(vec3(-30.0f, 30.0f, -30.0f));
		c_cam.set_springiness(0.5f);

		//set target camera properties
		t_cam.set_position(vec3(-10.0f, 30.0f, 50.0f));
		t_cam.set_projection(half_pi<float>(), renderer::get_screen_aspect(), 0.1f, 4000.0f);
	}

	//spawn skybox
	{
		skybox = mesh(geometry_builder::create_box());
		skybox.get_transform().scale = vec3(3000, 3000, 3000);
	}
	

	//skybox cubemap textures
	{
		array<string, 6> filenames = { "res/textures/rainforest_ft.png", "res/textures/rainforest_bk.png", "res/textures/rainforest_up.png", "res/textures/rainforest_dn.png",
			"res/textures/rainforest_rt.png", "res/textures/rainforest_lf.png" };
		sky_cube = cubemap(filenames);
	}

	return true;
}

bool update(float delta_time)
{

	//choose free
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		c1 = true;
		c2 = false;
		c3 = false;
	}
	//choose chase
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		c1 = false;
		c2 = true;
		c3 = false;
	}
	//chose static
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3))
	{
		c1 = false;
		c2 = false;
		c3 = true;
	}



	//free cam update
	if (c1)
	{

		// The ratio of pixels to rotation - remember the fov
		static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
		static double ratio_height =
			(quarter_pi<float>() *
			(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
			static_cast<float>(renderer::get_screen_height());

		double current_x;
		double current_y;
		// *********************************
		// Get the current cursor position
		glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
		// Calculate delta of cursor positions from last frame
		double delta_x = current_x - cursor_x;
		double delta_y = current_y - cursor_y;
		// Multiply deltas by ratios - gets actual change in orientation
		delta_x *= ratio_width;
		delta_y *= ratio_height;
		// Rotate cameras by delta
		// delta_y - x-axis rotation
		// delta_x - y-axis rotation
		f_cam.rotate(delta_x, -delta_y);
		// Use keyboard to move the camera - WSAD
		vec3 forward = vec3(0), back = vec3(0), left = vec3(0), right = vec3(0), up = vec3(0), down = vec3(0);
		vec3 total = vec3(0);

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
			forward = vec3(0.0f, 0.0f, 1.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
			back = vec3(0.0f, 0.0f, -1.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
			left = vec3(-1.0f, 0.0f, 0.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
			right = vec3(1.0f, 0.0f, 0.0f);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE))
			up = vec3(0, 1.0f, 0);
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL))
			left = vec3(0, -1, 0);

		// Move camera
		total = forward + back + left + right + up + down;
		if (total != vec3(0))
			f_cam.move(normalize(total)* delta_time * 10.0f);
		// Update the camera
		f_cam.update(delta_time);
		// Update cursor pos
		cursor_x = current_x;
		cursor_y = current_y;

		skybox.get_transform().position = f_cam.get_position();
	}

	//chase cam update
	if (c2)
	{
		//Lock camera to target mesh
		c_cam.move(meshes["dino"].get_transform().position + vec3(0.0f, 20.0f, 0.0f), eulerAngles(meshes["dino"].get_transform().orientation));
		c_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 3000.0f);


		// Rotate mesh on y axis with Q/E
		if (glfwGetKey(renderer::get_window(), 'Q'))
			meshes["dino"].get_transform().rotate(vec3(0, half_pi<float>(), 0)*delta_time);
		if (glfwGetKey(renderer::get_window(), 'E'))
			meshes["dino"].get_transform().rotate(vec3(0, -half_pi<float>(), 0)*delta_time);



		// Move the target_mesh - WSAD
		if (glfwGetKey(renderer::get_window(), 'W'))
			meshes["dino"].get_transform().position.z += 25.0f * delta_time;
		if (glfwGetKey(renderer::get_window(), 'S'))
			meshes["dino"].get_transform().position.z -= 25.0f * delta_time;
		if (glfwGetKey(renderer::get_window(), 'A'))
			meshes["dino"].get_transform().position.x -= 25.0f * delta_time;
		if (glfwGetKey(renderer::get_window(), 'D'))
			meshes["dino"].get_transform().position.x += 25.0f * delta_time;


		c_cam.update(delta_time);
		skybox.get_transform().position = c_cam.get_position();
	}

	//target cam update
	if (c3)
	{
		t_cam.set_target(meshes["dino"].get_transform().position);
		t_cam.update(delta_time);
		skybox.get_transform().position = t_cam.get_position();
	}

	return true;
}

bool render() {

	//render skybox 
	{
		mat4 M = skybox.get_transform().get_transform_matrix();
		mat4 P;
		mat4 V;
		//get different projections from different cameras
		if (c1)
		{
			V = f_cam.get_view();
			P = f_cam.get_projection();
		}
		if (c2)
		{
			V = c_cam.get_view();
			P = c_cam.get_projection();
		}
		if (c3)
		{
			V = t_cam.get_view();
			P = t_cam.get_projection();
		}


		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		renderer::bind(sky_eff);
		//remember auto appears unsafe for this use
		mat4 MVP = P * V * M;
		//set cubemap uniform
		glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		renderer::bind(sky_cube, 0);
		glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);
		renderer::render(skybox);
		cout << "skybox" << endl;

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
	}

	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = f_cam.get_view();
		auto P = f_cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// *********************************
		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform - remember - 3x3 matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		// Bind material
		renderer::bind(m.get_material(), "mat");
		// Bind light
		renderer::bind(sun_dir, "light");
		// Set eye position - Get this from active camera
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(f_cam.get_position()));
		// Render mesh
		renderer::render(m);
		// *********************************
	}

	
	return true;
}

void main() {
	// Create application
	app application("Graphics Coursework");
	application.set_initialise(initialise);
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}