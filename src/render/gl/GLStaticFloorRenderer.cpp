#include <glm/gtc/type_ptr.hpp>

#include "GLStaticFloorRenderer.hpp"

using namespace win::gl;

GLStaticFloorRenderer::GLStaticFloorRenderer(win::AssetRoll &roll, const FloorTextureCollection &floortextures)
	: program(win::load_gl_shaders(roll["shader/static_floor.vert"], roll["shader/static_floor.frag"]))
	, floortextures(floortextures)
{
	glUseProgram(program.get());
	uniform_view_projection = glGetUniformLocation(program.get(), "view_projection");
	if (uniform_view_projection == -1) win::bug("no view projection");

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position_texcoord.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glBindBuffer(GL_ARRAY_BUFFER, layer.get());
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, 0, NULL);
}

void GLStaticFloorRenderer::set_view_projection(const glm::mat4 &view)
{
	glUseProgram(program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(view));
}

std::vector<std::uint16_t> GLStaticFloorRenderer::load(const std::vector<Renderable> &renderables)
{
	std::vector<std::uint16_t> results;

	static const float verts[] =
	{
		-0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f
	};

	static const auto ident = glm::identity<glm::mat4>();

	std::vector<float> position_texcoord_data;
	std::vector<std::uint8_t> layer_data;
	std::vector<std::uint16_t> index_data;

	int count = 0;
	for (const auto &renderable : renderables)
	{
		if (count * 6 > std::numeric_limits<std::uint16_t>::max())
			win::bug("static floor index overflow");

		const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 0.0f));
		const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f), renderable.y + (renderable.h / 2.0f), 0.0f));

		for (int i = 0; i < 4; ++i)
		{
			const glm::vec4 raw(verts[(i * 4) + 0], verts[(i * 4) + 1], 0.0f, 1.0f);
			const glm::vec2 tc(verts[(i * 4) + 2], verts[(i * 4) + 3]);
			const auto transformed = translate * scale * raw;

			position_texcoord_data.push_back(transformed.x);
			position_texcoord_data.push_back(transformed.y);
			position_texcoord_data.push_back(tc.s * renderable.w);
			position_texcoord_data.push_back(tc.t * renderable.h);
		}

		const int layer_index = floortextures.get_layer(renderable.texture);
		if (layer_index > std::numeric_limits<std::uint8_t>::max())
			win::bug("overflow");

		layer_data.push_back(layer_index);
		layer_data.push_back(layer_index);
		layer_data.push_back(layer_index);
		layer_data.push_back(layer_index);

		const int base_index = count * 4;
		index_data.push_back(base_index + 0);
		index_data.push_back(base_index + 1);
		index_data.push_back(base_index + 2);
		index_data.push_back(base_index + 0);
		index_data.push_back(base_index + 2);
		index_data.push_back(base_index + 3);

		results.push_back(count * 6);
		++count;
	}

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position_texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_texcoord_data.size(), position_texcoord_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, layer.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint8_t) * layer_data.size(), layer_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint16_t) * index_data.size(), index_data.data(), GL_STATIC_DRAW);

	return results;
}

void GLStaticFloorRenderer::add(std::uint16_t base_vertex)
{
	scene.push_back(base_vertex);
}

void GLStaticFloorRenderer::flush()
{
	if (scene.empty())
		return;

	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	for (const auto bv : scene)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(sizeof(std::uint16_t) * bv));

#ifndef NDEBUG
	win::gl_check_error();
#endif

	scene.clear();
}
