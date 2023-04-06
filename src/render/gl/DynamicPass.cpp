#include <glm/gtc/type_ptr.hpp>

#include "GL.hpp"
#include "DynamicPass.hpp"

using namespace win::gl;

DynamicPass::DynamicPass(win::AssetRoll &roll)
	: atlas(roll["texture/gameplay.atlas"], win::GLAtlas::Mode::linear)
{
	// shader
	program = win::load_gl_shaders(roll["shader/dynamic.vert"], roll["shader/dynamic.frag"]);
	glUseProgram(program);
	uniform_projection = get_uniform(program, "projection");
	uniform_view = get_uniform(program, "view");

	// vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	std::vector<float> verts;

	// fill up the verts
	const int gameplay_textures = (int)Texture::gameplay_end - (int)Texture::gameplay_start;
	for (int i = 0; i < gameplay_textures; ++i)
	{
		const win::AtlasItem &item = atlas.item(i);

		float v[] =
		{
			-0.5f, 0.5f, item.x1, item.y2,
			-0.5f, -0.5f, item.x1, item.y1,
			0.5f, -0.5f, item.x2, item.y1,
			0.5f, 0.5f, item.x2, item.y2
		};

		for (const float f : v)
			verts.push_back(f);
	}

	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts.size(), verts.data(), GL_STATIC_DRAW);

	// draw ids

	std::vector<unsigned int> drawids;
	for (int i = 0; i < ubo_size; ++i)
		drawids.push_back(i);

	glGenBuffers(1, &vbo_drawids);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_drawids);
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, NULL);
	glVertexAttribDivisor(3, 1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * drawids.size(), drawids.data(), GL_STATIC_DRAW);

	// element buffer

	const unsigned short elements[] =
	{
		0, 1, 2, 0, 2, 3
	};

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// transforms uniform block

	glGenBuffers(1, &ubo_transforms);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_transforms);
	const auto transforms_index = glGetUniformBlockIndex(program, "transforms");
	if (transforms_index == GL_INVALID_INDEX) win::bug("no transforms");
	glUniformBlockBinding(program, transforms_index, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_transforms);

	glBufferStorage(GL_UNIFORM_BUFFER, sizeof(float) * 32, NULL, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);
	auto mem = (unsigned char*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(float) * 32, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);

	const auto m1 = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f));
	const auto m2 = glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 0.0f));
	memcpy(mem, glm::value_ptr(m1), sizeof(float) * 16);
	memcpy(mem + (sizeof(float) * 16), glm::value_ptr(m2), sizeof(float) * 16);
}

DynamicPass::~DynamicPass()
{
	glDeleteBuffers(1, &ubo_transforms);
	glDeleteBuffers(1, &vbo_drawids);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
}

void DynamicPass::set_projection(const glm::mat4 &projection) const
{
	glUseProgram(program);
	glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));
}

void DynamicPass::set_view(const glm::mat4 &view) const
{
	glUseProgram(program);
	glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(view));
}

void DynamicPass::set_renderables(const std::vector<Renderable> &renderables)
{
}

void DynamicPass::draw()
{
	glUseProgram(program);

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, atlas.texture());

	DrawElementsIndirectCommand cmd[2];
	cmd[0].count = 6;
	cmd[0].instance_count = 1;
	cmd[0].first_index = 0;
	cmd[0].base_vertex = 0;
	cmd[0].base_instance = 0;
	cmd[1].count = 6;
	cmd[1].instance_count = 1;
	cmd[1].first_index = 0;
	cmd[1].base_vertex = 4;
	cmd[1].base_instance = 1;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, &cmd, 2, 0);
}
