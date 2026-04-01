#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#include "GLAtlasRenderer.hpp"
#include "GLConstants.hpp"

using namespace win::gl;

static std::uint16_t to_ushort(float f)
{
	return (std::uint16_t)(f * std::numeric_limits<std::uint16_t>::max());
}

GLAtlasRenderer::GLAtlasRenderer(win::AssetRoll &roll, const TextureAssetMap &texture_map)
{
	init_atlases(roll, texture_map);

	program = win::GLProgram(win::gl_load_shaders(roll["shader/static_atlas.vert"], roll["shader/static_atlas.frag"]));
	glUseProgram(program.get());
	uniform_view_projection = get_uniform(program, "view_projection");
	const auto uniform_tex = get_uniform(program, "tex");
	glUniform1i(uniform_tex, GLConstants::atlas_texture_unit - GL_TEXTURE0);

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, 0, NULL);

	check_error();
}

void GLAtlasRenderer::set_view_projection(const glm::mat4 &view_projection)
{
	glUseProgram(program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(view_projection));
}

void GLAtlasRenderer::render(const std::vector<std::uint16_t> &ids)
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	for (const auto base_vertex : ids)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, base_vertex, 4);
	}

	check_error();
}

std::vector<std::uint16_t> GLAtlasRenderer::load(const std::vector<Renderable> &renderables, const TextureAssetMap &texture_map)
{
	std::vector<std::uint16_t> results;

	// clang-format off
	const float verts[] =
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, 0.5f,
		0.5f, -0.5f
	};
	// clang-format on

	const auto ident = glm::identity<glm::mat4>();

	std::vector<float> position_data;
	std::vector<std::uint16_t> texcoord_data;
	std::vector<std::uint16_t> index_data;

	int count = 0;
	for (const auto &renderable : renderables)
	{
		const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f), renderable.y + (renderable.h / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 0.0f));
		const auto rotate = glm::rotate(ident, renderable.rot, glm::vec3(0.0f, 0.0f, 1.0f));

		for (int i = 0; i < 4; ++i)
		{
			const glm::vec4 raw(verts[i * 2], verts[i * 2 + 1], 0.0f, 1.0f);
			const auto transformed = translate * rotate * scale * raw;

			position_data.push_back(transformed.x);
			position_data.push_back(transformed.y);
		}

		const auto &atlas = *atlas_map.at((int)renderable.texture);
		const auto &atlas_item = atlas.item(texture_map[renderable.texture].atlas_index);

		texcoord_data.push_back(to_ushort(atlas_item.x1));
		texcoord_data.push_back(to_ushort(atlas_item.y2));
		texcoord_data.push_back(to_ushort(atlas_item.x1));
		texcoord_data.push_back(to_ushort(atlas_item.y1));
		texcoord_data.push_back(to_ushort(atlas_item.x2));
		texcoord_data.push_back(to_ushort(atlas_item.y2));
		texcoord_data.push_back(to_ushort(atlas_item.x2));
		texcoord_data.push_back(to_ushort(atlas_item.y1));

		results.push_back(count * 4);
		++count;
	}

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_data.size(), position_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint16_t) * texcoord_data.size(), texcoord_data.data(), GL_STATIC_DRAW);

	return results;
}

/*
void GLAtlasRenderer::add(std::uint16_t base_vertex)
{
	scene.push_back(base_vertex);
}
*/

void GLAtlasRenderer::init_atlases(win::AssetRoll &roll, const TextureAssetMap &map)
{
	struct Atlas
	{
		Atlas(const char *name, win::GLAtlas *atlas)
			: name(name)
			, atlas(atlas)
		{}

		const char *name;
		win::GLAtlas *atlas;
	};

	std::vector<Atlas> atlasmap;

	for (int i = 0; i < map.size(); ++i)
	{
		const auto &t = map[(Texture)i];

		if (t.atlas_index == -1)
		{
			atlas_map.push_back(NULL);
			continue;
		}

		auto found = std::find_if(atlasmap.begin(), atlasmap.end(), [&t](const Atlas &a){ return !strcmp(a.name, t.asset_path); });

		if (found == atlasmap.end())
		{
			auto &atlas = atlases.add(roll[t.asset_path], win::GLAtlas::Mode::linear, GLConstants::atlas_texture_unit);
			atlasmap.emplace_back(t.asset_path, &atlas);
			atlas_map.push_back(&atlas);
		}
		else
			atlas_map.push_back(found->atlas);
	}
}
