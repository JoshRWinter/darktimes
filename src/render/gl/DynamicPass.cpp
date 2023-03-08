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
	uniform_model = get_uniform(program, "model");

	// vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// main buffer
	const float verts[] =
	{
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f,  -0.5f,
		0.5f, 0.5f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	// element buffer
	const int elements[] =
	{
		0, 1, 2, 0, 2, 3
	};

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// texcoords
	glGenBuffers(1, &vbo_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

DynamicPass::~DynamicPass()
{
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
}

void DynamicPass::set_projection(const glm::mat4 &projection)
{
	glUseProgram(program);
	glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));
}

void DynamicPass::set_view(const glm::mat4 &view)
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
	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));

	const win::AtlasItem item = atlas.item(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
	const float tc[] =
	{
		item.x1, item.y2,
		item.x1, item.y1,
		item.x2, item.y1,
		item.x2, item.y2
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(tc), tc, GL_DYNAMIC_DRAW);

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, atlas.texture());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}
