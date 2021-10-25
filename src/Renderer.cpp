#include <GL/gl.h>

#include <win.h>
#include "Renderer.hpp"

static float verts[] =
{
	-0.5f, 0.5f,
	0.5f, -0.5f,
	-0.5f, -0.5f,

	-0.5f, 0.5f,
	0.5f, -0.5f,
	0.5f, 0.5f
};

Renderer::Renderer(int iwidth, int iheight, float left, float right, float bottom, float top, win::AssetRoll &roll)
	: font_renderer(iwidth, iheight, left, right, bottom, top)
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

	shader_wall = win::load_shaders(roll["shader/wall.vert"], roll["shader/wall.frag"]);
	glGenVertexArrays(1, &vao_wall);
	glGenBuffers(1, &vbo_wall);

	glBindVertexArray(vao_wall);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_wall);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &vbo_wall);
	glDeleteVertexArrays(1, &vao_wall);
	glDeleteShader(shader_wall);
}

void Renderer::computeframe()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader_wall);
	glBindVertexArray(vao_wall);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_wall);
	glDrawArrays(GL_TRIANGLES, 0, 6);

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
