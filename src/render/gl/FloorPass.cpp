#include <string.h>
#include <glm/gtc/type_ptr.hpp>

#include <win/GL.hpp>
#include <win/Targa.hpp>

#include "../../Texture.hpp"
#include "GL.hpp"
#include "FloorPass.hpp"

static std::vector<win::Targa> get_floor_textures(win::AssetRoll &roll)
{
	std::vector<win::Targa> textures;

	for (int i = (int)Texture::level_floor_start; i <= (int)Texture::level_floor_end; ++i)
	{
		const std::string name = "texture/floor" + std::to_string((i - (int)Texture::level_floor_start) + 1) + ".tga";
		textures.emplace_back(roll[name.c_str()]);
	}

	return textures;
}

static std::vector<float> get_floor_verts(const std::vector<Renderable> &floors)
{
	std::vector<float> verts;

	const float floor_texture_tile_size = 0.5f;

	for (const auto &floor : floors)
	{
		const float x_magnitude = floor.w / floor_texture_tile_size;
		const float y_magnitude = floor.h / floor_texture_tile_size;

		verts.push_back(floor.x); verts.push_back(floor.y + floor.h); verts.push_back(0.0f); verts.push_back(y_magnitude); verts.push_back((float)floor.texture);
		verts.push_back(floor.x); verts.push_back(floor.y); verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back((float)floor.texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y); verts.push_back(x_magnitude); verts.push_back(0.0f); verts.push_back((float)floor.texture);

		verts.push_back(floor.x); verts.push_back(floor.y + floor.h); verts.push_back(0.0f); verts.push_back(y_magnitude); verts.push_back((float)floor.texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y); verts.push_back(x_magnitude); verts.push_back(0.0f); verts.push_back((float)floor.texture);
		verts.push_back(floor.x + floor.w); verts.push_back(floor.y + floor.h); verts.push_back(x_magnitude); verts.push_back(y_magnitude); verts.push_back((float)floor.texture);
	}

	return verts;
}

FloorPass::FloorPass(win::AssetRoll &roll)
{
	floorvert_count = 0;

	const std::vector<win::Targa> floortargas = get_floor_textures(roll);
	const unsigned long long floortexture_bytesize = floortargas.at(0).width() * floortargas.at(0).height() * 4;
	auto floortexture_data = std::make_unique<unsigned char[]>(floortexture_bytesize * floortargas.size());
	unsigned long long floortexture_index = 0;
	for (auto &targa : floortargas)
	{
		memcpy(floortexture_data.get() + floortexture_index, targa.data(), floortexture_bytesize);
		floortexture_index += floortexture_bytesize;
	}
	glGenTextures(1, &floortextures);
	glBindTexture(GL_TEXTURE_2D_ARRAY, floortextures);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, floortargas.at(0).width(), floortargas.at(0).height(), floortargas.size(), 0, GL_BGRA, GL_UNSIGNED_BYTE, floortexture_data.get());

	shader = win::load_gl_shaders(roll["shader/floor.vert"], roll["shader/floor.frag"]);
	glUseProgram(shader);

	uniform_projection = get_uniform(shader, "projection");
	uniform_view = get_uniform(shader, "view");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 20, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 20, (void*)8);
}

FloorPass::~FloorPass()
{
	glDeleteTextures(1, &floortextures);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(shader);
}

void FloorPass::draw()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, floortextures);
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, floorvert_count / 4);
}

void FloorPass::set_floors(const std::vector<Renderable> &floors)
{
	const auto &floor_verts = get_floor_verts(floors);
	floorvert_count = floor_verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floorvert_count, floor_verts.data(), GL_STATIC_DRAW);
}

void FloorPass::set_projection(const glm::mat4 &projection_matrix)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniform_projection, 1, false, glm::value_ptr(projection_matrix));
}

void FloorPass::set_view(const glm::mat4 &view_matrix)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniform_view, 1, false, glm::value_ptr(view_matrix));
}
