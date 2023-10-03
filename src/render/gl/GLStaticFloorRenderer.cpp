#include <glm/gtc/type_ptr.hpp>

#include "GLStaticFloorRenderer.hpp"

using namespace win::gl;

GLStaticFloorRenderer::GLStaticFloorRenderer(win::AssetRoll &roll)
	: program(win::load_gl_shaders(roll["shader/static_floor.vert"], roll["shader/static_floor.frag"]))
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

std::vector<const void*> GLStaticFloorRenderer::load_static_floors(const std::vector<Renderable> &statics, const std::vector<int> &floor_layer_map)
{
	loaded_static_floors.clear();
	loaded_static_floors.reserve(statics.size());

	std::vector<float> position_texcoord_data(4 * 4 * statics.size());
	std::vector<std::uint8_t> layer_data(4 * 1 * statics.size());
	std::vector<std::uint16_t> index_data(6 * 1 * statics.size());

	position_texcoord_data.clear();
	layer_data.clear();
	index_data.clear();

	const float verts[] =
	{
		-0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f
	};

	const auto ident = glm::identity<glm::mat4>();

	int index = 0;
	for (const auto &floor : statics)
	{
		const auto scale = glm::scale(ident, glm::vec3(floor.w, floor.h, 0.0f));
		const auto translate = glm::translate(ident, glm::vec3(floor.x + (floor.w / 2.0f), floor.y + (floor.h / 2.0f), 0.0f));

		for (int i = 0; i < 4; ++i)
		{
			const glm::vec4 raw(verts[(i * 4) + 0], verts[(i * 4) + 1], 0.0f, 1.0f);
			const glm::vec2 tc(verts[(i * 4) + 2], verts[(i * 4) + 3]);
			const auto transformed = translate * scale * raw;

			position_texcoord_data.push_back(transformed.x);
			position_texcoord_data.push_back(transformed.y);
			position_texcoord_data.push_back(tc.s * floor.w);
			position_texcoord_data.push_back(tc.t * floor.h);
		}

		const int layer = floor_layer_map.at(floor.texture);
		layer_data.push_back(layer);
		layer_data.push_back(layer);
		layer_data.push_back(layer);
		layer_data.push_back(layer);

		const int base_index = index * 4;
		index_data.push_back(base_index + 0);
		index_data.push_back(base_index + 1);
		index_data.push_back(base_index + 2);
		index_data.push_back(base_index + 0);
		index_data.push_back(base_index + 2);
		index_data.push_back(base_index + 3);

		loaded_static_floors.emplace_back().vertex_offset = index * 6;

		++index;
	}

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position_texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_texcoord_data.size(), position_texcoord_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, layer.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint8_t) * layer_data.size(), layer_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint16_t) * index_data.size(), index_data.data(), GL_STATIC_DRAW);

	std::vector<const void*> result(loaded_static_floors.size());
	result.clear();
	for (const auto &f : loaded_static_floors)
		result.push_back(&f);

	return result;
}

void GLStaticFloorRenderer::add(const void *v)
{
	const auto obj = (GLStaticFloor*)v;
	scene.push_back(obj);
}

void GLStaticFloorRenderer::flush()
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	for (const auto &x : scene)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(sizeof(std::uint16_t) * x->vertex_offset));

	scene.clear();
}
