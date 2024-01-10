#include <glm/gtc/type_ptr.hpp>

#include "GLStaticAtlasRenderer.hpp"

using namespace win::gl;

static std::uint16_t to_ushort(float f)
{
	return (std::uint16_t)(f * std::numeric_limits<std::uint16_t>::max());
}

GLStaticAtlasRenderer::GLStaticAtlasRenderer(win::AssetRoll &roll, const TextureAssetMap &texture_map, AtlasTextureCollection &atlas_textures)
	: program(win::load_gl_shaders(roll["shader/static_atlas.vert"], roll["shader/static_atlas.frag"]))
	, texture_map(texture_map)
	, atlas_textures(atlas_textures)
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

std::uint16_t GLStaticAtlasRenderer::load(const Renderable &renderable)
{
	const float verts[] =
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f
	};

	const auto ident = glm::identity<glm::mat4>();

	const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f),
														   renderable.y + (renderable.h / 2.0f), 0.0f));
	const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 0.0f));

	for (int i = 0; i < 4; ++i)
	{
		const glm::vec4 raw(verts[(i * 2) + 0], verts[(i * 2) + 1], 0.0f, 1.0f);
		const auto transformed = translate * scale * raw;

		staging.position.push_back(transformed.x);
		staging.position.push_back(transformed.y);
	}

	const auto &atlas = atlas_textures.get_atlas(renderable.texture);
	const auto &atlas_item = atlas.item(texture_map[renderable.texture].atlas_index);
	staging.texcoord.push_back(to_ushort(atlas_item.x1));
	staging.texcoord.push_back(to_ushort(atlas_item.y2));
	staging.texcoord.push_back(to_ushort(atlas_item.x1));
	staging.texcoord.push_back(to_ushort(atlas_item.y1));
	staging.texcoord.push_back(to_ushort(atlas_item.x2));
	staging.texcoord.push_back(to_ushort(atlas_item.y1));
	staging.texcoord.push_back(to_ushort(atlas_item.x2));
	staging.texcoord.push_back(to_ushort(atlas_item.x2));

	const auto base_index = staging.count * 4;
	staging.index.push_back(base_index + 0);
	staging.index.push_back(base_index + 1);
	staging.index.push_back(base_index + 2);
	staging.index.push_back(base_index + 0);
	staging.index.push_back(base_index + 2);
	staging.index.push_back(base_index + 3);

	++staging.count;

	if (staging.count * 6 > std::numeric_limits<std::uint16_t>::max())
		win::bug("static atlas index overflow");

	return (staging.count - 1) * 6;
}

void GLStaticAtlasRenderer::finalize()
{
	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * staging.position.size(), staging.position.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint16_t) * staging.texcoord.size(), staging.texcoord.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint16_t) * staging.index.size(), staging.index.data(), GL_STATIC_DRAW);
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
