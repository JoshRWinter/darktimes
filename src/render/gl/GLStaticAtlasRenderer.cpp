#include <glm/gtc/type_ptr.hpp>

#include "GLStaticAtlasRenderer.hpp"

using namespace win::gl;

static std::uint16_t to_ushort(float f)
{
	return (std::uint16_t)(f * std::numeric_limits<std::uint16_t>::max());
}

GLStaticAtlasRenderer::GLStaticAtlasRenderer(win::AssetRoll &roll)
	: program(win::load_gl_shaders(roll["shader/static_atlas.vert"], roll["shader/static_atlas.frag"]))
{
	glUseProgram(program.get());
	uniform_view_projection = glGetUniformLocation(program.get(), "view_projection");
	if (uniform_view_projection == -1) win::bug("no uniform view_projection");

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, 0, NULL);
}

void GLStaticAtlasRenderer::set_view_projection(const glm::mat4 &view_projection)
{
	glUseProgram(program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(view_projection));
}

std::vector<std::uint16_t> GLStaticAtlasRenderer::load(const std::vector<Renderable> &renderables, const GLAtlasTextureCollection &atlases, const TextureAssetMap &texture_map)
{
	std::vector<std::uint16_t> results;

	const float verts[] =
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f
	};

	const auto ident = glm::identity<glm::mat4>();

	std::vector<float> position_data;
	std::vector<std::uint16_t> texcoord_data;
	std::vector<std::uint16_t> index_data;

	int count = 0;
	for (const auto &renderable : renderables)
	{
		if (count * 6 > std::numeric_limits<std::uint16_t>::max())
			win::bug("static atlas index overflow");

		const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f), renderable.y + (renderable.h / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 0.0f));

		for (int i = 0; i < 4; ++i)
		{
			const glm::vec4 raw(verts[(i * 2) + 0], verts[(i * 2) + 1], 0.0f, 1.0f);
			const auto transformed = translate * scale * raw;

			position_data.push_back(transformed.x);
			position_data.push_back(transformed.y);
		}

		const auto &atlas = atlases.get_atlas(renderable.texture);
		const auto &atlas_item = atlas.item(texture_map[renderable.texture].atlas_index);
		texcoord_data.push_back(to_ushort(atlas_item.x1));
		texcoord_data.push_back(to_ushort(atlas_item.y2));
		texcoord_data.push_back(to_ushort(atlas_item.x1));
		texcoord_data.push_back(to_ushort(atlas_item.y1));
		texcoord_data.push_back(to_ushort(atlas_item.x2));
		texcoord_data.push_back(to_ushort(atlas_item.y1));
		texcoord_data.push_back(to_ushort(atlas_item.x2));
		texcoord_data.push_back(to_ushort(atlas_item.y2));

		const auto base_index = count * 4;
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

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_data.size(), position_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint16_t) * texcoord_data.size(), texcoord_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint16_t) * index_data.size(), index_data.data(), GL_STATIC_DRAW);

	return results;
}

void GLStaticAtlasRenderer::add(std::uint16_t base_vertex)
{
	scene.push_back(base_vertex);
}

void GLStaticAtlasRenderer::flush()
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
