#include <win.h>
#include <GL/gl.h>

#include "renderer.hpp"

static int get_uniform(unsigned program, const char *name)
{
	auto result = glGetUniformLocation(program, name);
	if (result == -1)
		win::bug(std::string("No uniform ") + name);

	return result;
}

Renderer::Renderer(int iwidth, int iheight, float left, float right, float bottom, float top, win::AssetRoll &roll)
	: wallvert_count(0)
	, font_renderer(iwidth, iheight, left, right, bottom, top)
	, font_debug(font_renderer, roll["font/NotoSansMono-Regular.ttf"], 0.25f)
	, font_ui(font_renderer, roll["font/CHE-THIS.TTF"], 0.7f)
	, last_fps_calc_time(std::chrono::high_resolution_clock::now())
	, accumulated_fps(0)
	, left(left)
	, right(right)
	, bottom(bottom)
	, top(top)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	strcpy(fpsindicator, "0");

	shader.wall = win::load_shaders(roll["shader/wall.vert"], roll["shader/wall.frag"]);
	glUseProgram(shader.wall);

	float matrix[16];
	win::init_ortho(matrix, left, right, bottom, top);
	uniform.wall.projection = get_uniform(shader.wall, "projection");
	glUniformMatrix4fv(uniform.wall.projection, 1, false, matrix);

	glGenVertexArrays(1, &vao.wall);
	glGenBuffers(1, &vbo.wall);

	glBindVertexArray(vao.wall);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.wall);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &vbo.wall);
	glDeleteVertexArrays(1, &vao.wall);
	glDeleteShader(shader.wall);
}

void Renderer::set_wall_verts(const std::vector<float> &wallverts)
{
	wallvert_count = wallverts.size();
	glBindBuffer(GL_ARRAY_BUFFER, vbo.wall);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wallvert_count, wallverts.data(), GL_STATIC_DRAW);
}

void Renderer::computeframe()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader.wall);
	glBindVertexArray(vao.wall);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.wall);
	glDrawArrays(GL_TRIANGLES, 0, wallvert_count / 2);

	if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - last_fps_calc_time).count() > 1000)
	{
		snprintf(fpsindicator, sizeof(fpsindicator), "%d", accumulated_fps);
		accumulated_fps = 0;
		last_fps_calc_time = std::chrono::high_resolution_clock::now();
	}
	else ++accumulated_fps;

	font_renderer.draw(font_debug, fpsindicator, left + 0.05f, top - font_debug.size(), win::Color(1.0f, 1.0f, 0.0f, 1.0f));
	font_renderer.draw(font_ui, "Dark Times", 0.0f, top - font_ui.size(), win::Color(1.0f, 1.0f, 1.0f, 1.0f), true);

#ifndef NDEBUG
	auto error = glGetError();
	if (error != 0)
		win::bug("GL error " + std::to_string(error));
#endif
}
