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

unsigned tex1;

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

	float matrix[16];
	win::init_ortho(matrix, left, right, bottom, top);

	// mode: wall
	mode.wall.shader = win::load_shaders(roll["shader/wall.vert"], roll["shader/wall.frag"]);
	glUseProgram(mode.wall.shader);

	mode.wall.uniform_projection = get_uniform(mode.wall.shader, "projection");
	glUniformMatrix4fv(mode.wall.uniform_projection, 1, false, matrix);

	glGenVertexArrays(1, &mode.wall.vao);
	glGenBuffers(1, &mode.wall.vbo);

	glBindVertexArray(mode.wall.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mode.wall.vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);

	// mode: floor
	mode.floor.shader = win::load_shaders(roll["shader/floor.vert"], roll["shader/floor.frag"]);
	glUseProgram(mode.floor.shader);

	mode.floor.uniform_projection = get_uniform(mode.floor.shader, "projection");
	glUniformMatrix4fv(mode.floor.uniform_projection, 1, false, matrix);

	glGenVertexArrays(1, &mode.floor.vao);
	glGenBuffers(1, &mode.floor.vbo);

	glBindVertexArray(mode.floor.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mode.floor.vbo);

	float floorverts[] =
	{
		-2.0f, 2.0f, 0.0f, 1.0f,
		-2.0f, -2.0, 0.0f, 0.0f,
		2.0f, -2.0f, 1.0f, 0.0f,

		-2.0f, 2.0f, 0.0f, 1.0f,
		2.0f, -2.0f, 1.0f, 0.0f,
		2.0f, 2.0f, 1.0f, 1.0f
	};

	unsigned char tex1pixels[] =
	{
		255, 255, 255, 255,
		0, 255, 0, 255,
		0, 255, 0, 255,
		255, 255, 255, 255,

		255, 255, 255, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
		255, 255, 255, 255,
	};

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 16, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 16, (void*)8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorverts), floorverts, GL_STATIC_DRAW);

	glGenTextures(1, &tex1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex1);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1pixels);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 2, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1pixels);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &mode.wall.vbo);
	glDeleteVertexArrays(1, &mode.wall.vao);
	glDeleteShader(mode.wall.shader);

	glDeleteBuffers(1, &mode.floor.vbo);
	glDeleteVertexArrays(1, &mode.floor.vao);
	glDeleteShader(mode.floor.shader);
}

void Renderer::set_wall_verts(const std::vector<float> &wallverts)
{
	wallvert_count = wallverts.size();
	glBindBuffer(GL_ARRAY_BUFFER, mode.wall.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wallvert_count, wallverts.data(), GL_STATIC_DRAW);
}

void Renderer::computeframe()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D_ARRAY, tex1);
	glUseProgram(mode.floor.shader);
	glBindVertexArray(mode.floor.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(mode.wall.shader);
	glBindVertexArray(mode.wall.vao);
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
