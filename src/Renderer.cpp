#include <win.h>
#include <GL/gl.h>

#include "Renderer.hpp"

#include <random>

static float basex = 2.0f, basey = 2.0f;

static float verts[] =
{
	basex + -0.5f, basey + 0.5f, 0.0f, 1.0f,
	basex + -0.5f, basey + -0.5f, 0.0f, 0.0f,
	basex + 0.5f, basey + -0.5f, 1.0f, 0.0f,
	basex + 0.5f, basey + 0.5f, 1.0f, 1.0f
};

static int elements[] =
{
	0, 1, 2, 0, 2, 3
};

static unsigned char pixels[10 * 10 * 4];
unsigned tex;

static int get_uniform(unsigned program, const char *name)
{
	auto result = glGetUniformLocation(program, name);
	if (result == -1)
		win::bug(std::string("No uniform ") + name);

	return result;
}

float rot = 0.0f;

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

	shader.wall = win::load_shaders(roll["shader/wall.vert"], roll["shader/wall.frag"]);
	glUseProgram(shader.wall);

	float matrix[16];
	win::init_ortho(matrix, left, right, bottom, top);
	uniform.wall.projection = get_uniform(shader.wall, "projection");
	uniform.wall.rot = get_uniform(shader.wall, "rotation");
	glUniformMatrix4fv(uniform.wall.projection, 1, false, matrix);

	glGenVertexArrays(1, &vao.wall);
	glGenBuffers(1, &vbo.wall);
	glGenBuffers(1, &ebo.wall);

	glBindVertexArray(vao.wall);

	glBindBuffer(GL_ARRAY_BUFFER, vbo.wall);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 16, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 16, (void*)8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.wall);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	std::mt19937 gen(time(NULL));

	for (int i = 0; i < 10 * 10 * 4; i += 4)
	{
		pixels[i + 0] = std::uniform_int_distribution(0, 255)(gen);
		pixels[i + 1] = std::uniform_int_distribution(0, 255)(gen);
		pixels[i + 2] = std::uniform_int_distribution(0, 255)(gen);
		pixels[i + 3] = 255;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 10, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &vbo.wall);
	glDeleteBuffers(1, &ebo.wall);
	glDeleteVertexArrays(1, &vao.wall);
	glDeleteShader(shader.wall);
}

void Renderer::computeframe()
{
	glClear(GL_COLOR_BUFFER_BIT);

	rot += 0.27f;

	glUseProgram(shader.wall);
	glUniform1f(uniform.wall.rot, rot);
	glBindVertexArray(vao.wall);
	glBindTexture(GL_TEXTURE_2D, tex);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

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
