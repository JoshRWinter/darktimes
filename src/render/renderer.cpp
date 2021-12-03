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

static std::vector<float> get_floor_verts(const std::vector<LevelFloor> &floors)
{
	std::vector<float> verts;

	for (const auto &floor : floors)
	{
		const float x_magnitude = floor.w / AssetManager::floor_texture_tile_size;
		const float y_magnitude = floor.h / AssetManager::floor_texture_tile_size;

		verts.push_back(floor.x); verts.push_back(floor.y + floor.h); verts.push_back(0.0f); verts.push_back(y_magnitude); verts.push_back(floor.texture);
		verts.push_back(floor.x); verts.push_back(floor.y); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(floor.texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y); verts.push_back(x_magnitude); verts.push_back(0.0f); verts.push_back(floor.texture);

		verts.push_back(floor.x); verts.push_back(floor.y + floor.h); verts.push_back(0.0f); verts.push_back(y_magnitude); verts.push_back(floor.texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y); verts.push_back(x_magnitude); verts.push_back(0.0f); verts.push_back(floor.texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y + floor.h); verts.push_back(x_magnitude); verts.push_back(y_magnitude); verts.push_back(floor.texture);
	}

	return verts;
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

Renderer::Renderer(int iwidth, int iheight, float left, float right, float bottom, float top, AssetManager &assetmanager)
	: font_renderer(iwidth, iheight, left, right, bottom, top)
	, font_debug(font_renderer, assetmanager["font/NotoSansMono-Regular.ttf"], 0.25f)
	, font_ui(font_renderer, assetmanager["font/CHE-THIS.TTF"], 0.7f)
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
	mode.wall.wallvert_count = 0;
	mode.wall.shader = win::load_shaders(assetmanager["shader/wall.vert"], assetmanager["shader/wall.frag"]);
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
	mode.floor.floorvert_count = 0;

	std::vector<win::Targa> floortargas = assetmanager.get_floor_textures();
	const unsigned long long floortexture_bytesize = floortargas.at(0).width() * floortargas.at(0).height() * 4;
	auto floortexture_data = std::make_unique<unsigned char[]>(floortexture_bytesize * floortargas.size());
	unsigned long long floortexture_index = 0;
	for (auto &targa : floortargas)
	{
		memcpy(floortexture_data.get() + floortexture_index, targa.data(), floortexture_bytesize);
		floortexture_index += floortexture_bytesize;
	}
	glGenTextures(1, &mode.floor.floortextures);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mode.floor.floortextures);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, floortargas.at(0).width(), floortargas.at(0).height(), floortargas.size(), 0, GL_BGRA, GL_UNSIGNED_BYTE, floortexture_data.get());

	mode.floor.shader = win::load_shaders(assetmanager["shader/floor.vert"], assetmanager["shader/floor.frag"]);
	glUseProgram(mode.floor.shader);

	mode.floor.uniform_projection = get_uniform(mode.floor.shader, "projection");
	glUniformMatrix4fv(mode.floor.uniform_projection, 1, false, matrix);

	glGenVertexArrays(1, &mode.floor.vao);
	glGenBuffers(1, &mode.floor.vbo);

	glBindVertexArray(mode.floor.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mode.floor.vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 20, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 20, (void*)8);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &mode.wall.vbo);
	glDeleteVertexArrays(1, &mode.wall.vao);
	glDeleteShader(mode.wall.shader);

	glDeleteTextures(1, &mode.floor.floortextures);
	glDeleteBuffers(1, &mode.floor.vbo);
	glDeleteVertexArrays(1, &mode.floor.vao);
	glDeleteShader(mode.floor.shader);
}

void Renderer::set_level_data(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, int seed)
{
	levelseed = std::to_string(seed);
	const auto &floor_verts = get_floor_verts(floors);
	mode.floor.floorvert_count = floor_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, mode.floor.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mode.floor.floorvert_count, floor_verts.data(), GL_STATIC_DRAW);

	const auto &wall_verts = get_wall_verts(walls);
	mode.wall.wallvert_count = wall_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, mode.wall.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mode.wall.wallvert_count, wall_verts.data(), GL_STATIC_DRAW);
}

void Renderer::computeframe()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D_ARRAY, mode.floor.floortextures);
	glUseProgram(mode.floor.shader);
	glBindVertexArray(mode.floor.vao);
	glDrawArrays(GL_TRIANGLES, 0, mode.floor.floorvert_count / 4);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(mode.wall.shader);
	glBindVertexArray(mode.wall.vao);
	glDrawArrays(GL_TRIANGLES, 0, mode.wall.wallvert_count / 2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - last_fps_calc_time).count() > 1000)
	{
		snprintf(fpsindicator, sizeof(fpsindicator), "%d", accumulated_fps);
		accumulated_fps = 0;
		last_fps_calc_time = std::chrono::high_resolution_clock::now();
	}
	else ++accumulated_fps;

	font_renderer.draw(font_debug, fpsindicator, left + 0.05f, top - font_debug.size(), win::Color(1.0f, 1.0f, 0.0f, 1.0f));
	font_renderer.draw(font_ui, "Dark Times", 0.0f, top - font_ui.size(), win::Color(1.0f, 1.0f, 1.0f, 1.0f), true);
	font_renderer.draw(font_debug, levelseed.c_str(), 0.0f, 0.0f, win::Color(1.0f, 1.0f, 0.0f, 1.0f));

#ifndef NDEBUG
	auto error = glGetError();
	if (error != 0)
		win::bug("GL error " + std::to_string(error));
#endif
}

void Renderer::set_center(float x, float y)
{
	const float left = -16.0f + x;
	const float right = 16.0f + x;
	const float bottom = -9.0f + y;
	const float top = 9.0f + y;

	float matrix[16];
	win::init_ortho(matrix, left, right, bottom, top);

	glUseProgram(mode.floor.shader);
	glUniformMatrix4fv(mode.floor.uniform_projection, 1, false, matrix);
	glUseProgram(mode.wall.shader);
	glUniformMatrix4fv(mode.wall.uniform_projection, 1, false, matrix);
}
