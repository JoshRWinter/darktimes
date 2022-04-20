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

GLRenderer::GLRenderer(const win::Area<float> &world_area, win::AssetRoll &roll)
	: floor_pass(roll)
	, prop_pass(roll)
{
	const glm::mat4 projection_matrix = glm::ortho(world_area.left, world_area.right, world_area.bottom, world_area.top);
	const glm::mat4 view_matrix = glm::translate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(1.0, 1.0f, 1.0f)), glm::vec3(0.2, 0.2, 0));

	floor_pass.set_projection(projection_matrix);
	floor_pass.set_view(view_matrix);
	prop_pass.set_projection(projection_matrix);
	prop_pass.set_view(view_matrix);

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
}

GLRenderer::~GLRenderer()
{
	glDeleteBuffers(1, &mode.wall.vbo);
	glDeleteVertexArrays(1, &mode.wall.vao);
	glDeleteShader(mode.wall.shader);
}

void GLRenderer::set_level_data(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, const std::vector<LevelProp> &props)
{
	floor_pass.set_floors(floors);
	prop_pass.set_props(props);

	const auto &wall_verts = get_wall_verts(walls);
	mode.wall.wallvert_count = wall_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, mode.wall.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mode.wall.wallvert_count, wall_verts.data(), GL_STATIC_DRAW);
}

void GLRenderer::send_frame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	floor_pass.draw();

	glUseProgram(mode.wall.shader);
	glBindVertexArray(mode.wall.vao);
	glDrawArrays(GL_TRIANGLES, 0, mode.wall.wallvert_count / 2);

	prop_pass.draw();

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
	prop_pass.set_view(view_matrix);

	glUseProgram(mode.wall.shader);
	glUniformMatrix4fv(mode.wall.uniform_view, 1, false, glm::value_ptr(view_matrix));
}
