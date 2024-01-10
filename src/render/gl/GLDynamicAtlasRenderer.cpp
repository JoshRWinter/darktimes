#include <glm/gtc/type_ptr.hpp>

#include "GLDynamicAtlasRenderer.hpp"

using namespace win::gl;

static std::uint16_t to_ushort(float f)
{
	return (std::uint16_t)(f * std::numeric_limits<std::uint16_t>::max());
}

GLDynamicAtlasRenderer::GLDynamicAtlasRenderer(win::AssetRoll &roll)
	: program(win::load_gl_shaders(roll["shader/dynamic_atlas.vert"], roll["shader/dynamic_atlas.frag"]))
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

void GLDynamicAtlasRenderer::set_view_projection(const glm::mat4 &view_projection)
{
	glUseProgram(program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(view_projection));
}

void GLDynamicAtlasRenderer::load_all(const GLAtlasTextureCollection &atlas_textures, const TextureAssetMap &texture_map)
{
	std::vector<float> position_data;
	std::vector<std::uint16_t> texcoord_data;
	std::vector<std::uint8_t> index_data;

	const float verts[] =
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f
	};

	int count = 0;
	for (int texture = 0; texture < (int)Texture::max_texture; ++texture)
	{
		if (texture_map[(Texture)texture].dynamic)
		{
			if (count * 6 > std::numeric_limits<std::uint8_t>::max())
				win::bug("dynamic atlas index overflow");

			position_data.push_back(verts[0]);
			position_data.push_back(verts[1]);
			position_data.push_back(verts[2]);
			position_data.push_back(verts[3]);
			position_data.push_back(verts[4]);
			position_data.push_back(verts[5]);
			position_data.push_back(verts[6]);
			position_data.push_back(verts[7]);

			const auto &atlas = atlas_textures.get_atlas((Texture)texture);
			const auto &atlas_item = atlas.item(texture_map[(Texture) texture].atlas_index);

			texcoord_data.push_back(to_ushort(atlas_item.x1));
			texcoord_data.push_back(to_ushort(atlas_item.y2));
			texcoord_data.push_back(to_ushort(atlas_item.x1));
			texcoord_data.push_back(to_ushort(atlas_item.y1));
			texcoord_data.push_back(to_ushort(atlas_item.x2));
			texcoord_data.push_back(to_ushort(atlas_item.y1));
			texcoord_data.push_back(to_ushort(atlas_item.x2));
			texcoord_data.push_back(to_ushort(atlas_item.y2));

			const int base = count * 4;
			index_data.push_back(base + 0);
			index_data.push_back(base + 1);
			index_data.push_back(base + 2);
			index_data.push_back(base + 0);
			index_data.push_back(base + 2);
			index_data.push_back(base + 3);

			base_vertex_map[texture] = count * 6;

			++count;
		}
		else
		{
			base_vertex_map[texture] = std::numeric_limits<std::uint16_t>::max();
		}
	}

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, position.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_data.size(), position_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord.get());
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint16_t) * texcoord_data.size(), texcoord_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint8_t) * index_data.size(), index_data.data(), GL_STATIC_DRAW);

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

void GLDynamicAtlasRenderer::add(const Renderable &renderable)
{
	scene.push_back(&renderable);
}

void GLDynamicAtlasRenderer::flush()
{
	if (scene.empty())
		return;

	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	for (const auto renderable : scene)
	{
#ifndef NDEBUG
		if (base_vertex_map[(int)renderable->texture] == std::numeric_limits<std::uint16_t>::max())
			win::bug("dynamic atlas invalid base_vertex");
#endif

		glUniform2f(uniform_position, renderable->x, renderable->y);
		glUniform2f(uniform_size, renderable->w, renderable->h);
		glUniform1f(uniform_rotation, renderable->rot);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*)(sizeof(std::uint8_t) * base_vertex_map[(int)renderable->texture]));
	}

	scene.clear();
}
