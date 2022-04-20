#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/win.hpp>
#include <win/gl.hpp>
#include <win/targa.hpp>
#include <win/utility.hpp>

#include "glrenderer.hpp"
#include "../texture.hpp"

static GLint get_uniform(GLuint program, const char *name)
{
	auto result = glGetUniformLocation(program, name);
	if (result == -1)
		win::bug(std::string("No uniform ") + name);

	return result;
}

static void get_quad_verts(float x, float y, float w, float h, std::array<float, 12> &verts)
{
	verts[0] = x; verts[1] = y + h;
	verts[2] = x; verts[3] = y;
	verts[4] = x + w; verts[5] = y;

	verts[6] = x; verts[7] = y + h;
	verts[8] = x + w; verts[9] = y;
	verts[10] = x + w; verts[11] = y + h;
}

static std::vector<float> get_wall_verts(const std::vector<LevelWall> &walls)
{
	std::vector<float> verts;

	for (const auto &wall : walls)
	{
		std::array<float, 12> qverts;
		get_quad_verts(wall.x, wall.y, wall.w, wall.h, qverts);

		for (auto f : qverts)
			verts.push_back(f);
	}

	return verts;
}

static std::vector<float> get_prop_verts(const std::vector<LevelProp> &props)
{
	std::vector<float> verts;

	for (const auto &prop : props)
	{
		std::array<float, 12> qverts;
		get_quad_verts(prop.x, prop.y, prop.width, prop.height, qverts);

		// excluder (debugging)
		std::array<float, 12> qverts2;
		get_quad_verts(prop.x - prop.excluder_padding_x, prop.y - prop.excluder_padding_y, prop.width + (prop.excluder_padding_x * 2.0f), prop.height + (prop.excluder_padding_y * 2.0f), qverts2);

		for (auto f : qverts)
			verts.push_back(f);

		for (auto f : qverts2)
			verts.push_back(f);
	}

	return verts;
}

GLRenderer::GLRenderer(const win::Area<float> &world_area, win::AssetRoll &roll)
	: floor_pass(roll)
{
	const glm::mat4 projection_matrix = glm::ortho(world_area.left, world_area.right, world_area.bottom, world_area.top);
	const glm::mat4 view_matrix = glm::translate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(1.0, 1.0f, 1.0f)), glm::vec3(0.2, 0.2, 0));

	floor_pass.set_projection(projection_matrix);
	floor_pass.set_view(view_matrix);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// mode: wall
	mode.wall.wallvert_count = 0;
	mode.wall.shader = win::load_gl_shaders(roll["shader/wall.vert"], roll["shader/wall.frag"]);
	glUseProgram(mode.wall.shader);

	mode.wall.uniform_projection = get_uniform(mode.wall.shader, "projection");
	glUniformMatrix4fv(mode.wall.uniform_projection, 1, false, glm::value_ptr(projection_matrix));
	const std::string wall_vertex_shader = roll["shader/wall.vert"].read_all_as_string();
	mode.wall.uniform_view = get_uniform(mode.wall.shader, "view");
	glUniformMatrix4fv(mode.wall.uniform_view, 1, false, glm::value_ptr(view_matrix));

	glGenVertexArrays(1, &mode.wall.vao);
	glGenBuffers(1, &mode.wall.vbo);

	glBindVertexArray(mode.wall.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mode.wall.vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);

	// mode: prop
	mode.prop.propvert_count = 0;

	mode.prop.shader = win::load_gl_shaders(roll["shader/prop.vert"], roll["shader/prop.frag"]);
	glUseProgram(mode.prop.shader);

	mode.prop.uniform_projection = get_uniform(mode.prop.shader, "projection");
	glUniformMatrix4fv(mode.prop.uniform_projection, 1, false, glm::value_ptr(projection_matrix));
	mode.prop.uniform_view = get_uniform(mode.prop.shader, "view");
	glUniformMatrix4fv(mode.prop.uniform_view, 1, false, glm::value_ptr(view_matrix));

	glGenVertexArrays(1, &mode.prop.vao);
	glBindVertexArray(mode.prop.vao);

	glGenBuffers(1, &mode.prop.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mode.prop.vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
}

GLRenderer::~GLRenderer()
{
	glDeleteBuffers(1, &mode.wall.vbo);
	glDeleteVertexArrays(1, &mode.wall.vao);
	glDeleteShader(mode.wall.shader);

	glDeleteBuffers(1, &mode.prop.vbo);
	glDeleteVertexArrays(1, &mode.prop.vao);
	glDeleteShader(mode.prop.shader);
}

void GLRenderer::set_level_data(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, const std::vector<LevelProp> &props)
{
	floor_pass.set_floors(floors);

	const auto &wall_verts = get_wall_verts(walls);
	mode.wall.wallvert_count = wall_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, mode.wall.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mode.wall.wallvert_count, wall_verts.data(), GL_STATIC_DRAW);

	const auto &prop_verts = get_prop_verts(props);
	mode.prop.propvert_count = prop_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, mode.prop.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mode.prop.propvert_count, prop_verts.data(), GL_STATIC_DRAW);
}

void GLRenderer::send_frame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	floor_pass.draw();

	glUseProgram(mode.wall.shader);
	glBindVertexArray(mode.wall.vao);
	glDrawArrays(GL_TRIANGLES, 0, mode.wall.wallvert_count / 2);

	glUseProgram(mode.prop.shader);
	glBindVertexArray(mode.prop.vao);
	glDrawArrays(GL_TRIANGLES, 0, mode.prop.propvert_count / 2);

#ifndef NDEBUG
	auto error = glGetError();
	if (error != 0)
		win::bug("GL error " + std::to_string(error));
#endif
}

void GLRenderer::set_center(float x, float y)
{
	const float zoom = 0.5f;
	const glm::mat4 view_matrix = glm::translate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(zoom, zoom, zoom)), glm::vec3(-x, -y, 0.0f));

	floor_pass.set_view(view_matrix);

	glUseProgram(mode.wall.shader);
	glUniformMatrix4fv(mode.wall.uniform_view, 1, false, glm::value_ptr(view_matrix));

	glUseProgram(mode.prop.shader);
	glUniformMatrix4fv(mode.prop.uniform_view, 1, false, glm::value_ptr(view_matrix));
}
