#include <glm/gtc/type_ptr.hpp>

#include "../../TextureDefinitions.hpp"
#include "GLDynamicAtlasRenderer.hpp"

using namespace win::gl;

static std::uint16_t to_ushort(float f)
{
	return (std::uint16_t)(f * std::numeric_limits<std::uint16_t>::max());
}

GLDynamicAtlasRenderer::GLDynamicAtlasRenderer(win::AssetRoll &roll)
	: program(win::load_gl_shaders(roll["shader/dynamic_atlas.vert"], roll["shader/dynamic_atlas.frag"]))
	, current_texture(std::numeric_limits<GLuint>::max())
{
	glUseProgram(program.get());
	uniform_view_projection = glGetUniformLocation(program.get(), "view_projection");
	if (uniform_view_projection == -1) win::bug("no uniform view_projection");
	uniform_position = glGetUniformLocation(program.get(), "position");
	if (uniform_position == -1) win::bug("no uniform position");
	uniform_size = glGetUniformLocation(program.get(), "size");
	if (uniform_size == -1) win::bug("no uniform size");
	uniform_rotation = glGetUniformLocation(program.get(), "rotation");
	if (uniform_rotation == -1) win::bug("no uniform rotation");

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, 0, NULL);
}

void GLDynamicAtlasRenderer::set_atlases(const std::vector<win::GLAtlas> &atlases)
{
	this->atlases = &atlases;
}

void GLDynamicAtlasRenderer::set_view_projection(const glm::mat4 &view_projection)
{
	glUseProgram(program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(view_projection));
}

std::vector<std::uint16_t> GLDynamicAtlasRenderer::load_dynamic_atlas_items(const std::vector<win::GLAtlas> &atlases)
{
	std::vector<std::uint16_t> loaded;

	glBindVertexArray(vao.get());

	std::vector<float> position_data;
	std::vector<std::uint16_t> texcoord_data;
	std::vector<std::uint8_t> index_data;

	position_data.reserve(2 * 4 * TextureDefinitions.atlas_items.size());
	texcoord_data.reserve(2 * 4 * TextureDefinitions.atlas_items.size());
	index_data.reserve(1 * 6 * TextureDefinitions.atlas_items.size());

	const float verts[] =
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f
	};

	int idx = 0;
	for (const int i : TextureDefinitions.atlas_items)
	{
		const auto &def = TextureDefinitions.textures[i];

		for (int j = 0; j < 4; ++j)
		{
			position_data.push_back(verts[(j * 2) + 0]);
			position_data.push_back(verts[(j * 2) + 1]);
		}

		const auto &item = atlases.at(def.atlas).item(def.atlas_index);

		texcoord_data.push_back(to_ushort(item.x1));
		texcoord_data.push_back(to_ushort(item.y2));
		texcoord_data.push_back(to_ushort(item.x1));
		texcoord_data.push_back(to_ushort(item.y1));
		texcoord_data.push_back(to_ushort(item.x2));
		texcoord_data.push_back(to_ushort(item.y1));
		texcoord_data.push_back(to_ushort(item.x2));
		texcoord_data.push_back(to_ushort(item.y2));

		const int base_index = idx * 4;
		index_data.push_back(base_index + 0);
		index_data.push_back(base_index + 1);
		index_data.push_back(base_index + 2);
		index_data.push_back(base_index + 0);
		index_data.push_back(base_index + 2);
		index_data.push_back(base_index + 3);

		if (base_index + 3 > std::numeric_limits<std::uint8_t>::max())
			win::bug("GLDynamicAtlasRenderer: need to increase index depth");

		loaded.push_back(idx * 6);

		++idx;
	}

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_data.size(), position_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint16_t) * texcoord_data.size(), texcoord_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint8_t) * index_data.size(), index_data.data(), GL_STATIC_DRAW);

	return loaded;
}

void GLDynamicAtlasRenderer::add(const Renderable &renderable, std::uint32_t base_vertex)
{
	if (TextureDefinitions.textures[renderable.texture].atlas == -1)
		win::bug("GLDynamicAtlasRenderer: must be atlas");

	scene.emplace_back(&renderable, base_vertex);
}

void GLDynamicAtlasRenderer::flush()
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	for (const auto &item : scene)
	{
		const auto needed_texture = atlases->operator[](TextureDefinitions.textures[item.renderable->texture].atlas).texture();
		if (needed_texture != current_texture)
		{
			glBindTexture(GL_TEXTURE_2D, needed_texture);
			current_texture = needed_texture;
		}

		glUniform2f(0, )

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*)(sizeof(std::uint8_t) * item.base_vertex));
	}
}
