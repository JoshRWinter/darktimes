#include <glm/gtc/type_ptr.hpp>

#include "GL.hpp"
#include "DynamicPass.hpp"

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

	const unsigned short elements[] =
	{
		0, 1, 2, 0, 2, 3
	};

	// element buffer

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// transforms uniform block

	glGenBuffers(1, &transforms);
	glBindBuffer(GL_UNIFORM_BUFFER, transforms);
	const auto transforms_index = glGetUniformBlockIndex(program, "transforms");
	if (transforms_index == GL_INVALID_INDEX) win::bug("no transforms");
	glUniformBlockBinding(program, transforms_index, 0);

	glBufferStorage(GL_UNIFORM_BUFFER, sizeof(float) * 16, NULL, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);
	auto mem = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);

	auto m1 = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f));
	memcpy(mem, glm::value_ptr(m1), sizeof(float) * 16);
}

DynamicPass::~DynamicPass()
{
	glDeleteBuffers(1, &transforms);
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
	const glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f));

	glUseProgram(program);

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, atlas.texture());

	DrawElementsIndirectCommand cmd;
	cmd.count = 6;
	cmd.instance_count = 1;
	cmd.first_index = 0;
	cmd.base_vertex = 0;
	cmd.base_instance = 0;

	win::gl_check_error();
	//glBindBuffer(GL_UNIFORM_BUFFER, transforms);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, &cmd, 1, 0);
	win::gl_check_error();
}
