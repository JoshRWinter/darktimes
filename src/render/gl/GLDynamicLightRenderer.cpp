#include "glm/gtc/type_ptr.hpp"

#include "GLDynamicLightRenderer.hpp"

using namespace win::gl;

static int get_uniform(const win::GLProgram &program, const char *name)
{
	auto loc = glGetUniformLocation(program.get(), name);
	if (loc == -1)
		win::bug("GLDynamicLightRenderer: no uniform " + std::string(name));

	return loc;
}

GLDynamicLightRenderer::GLDynamicLightRenderer(win::AssetRoll &roll)
{
	program = win::GLProgram(win::load_gl_shaders(roll["shader/dynamic_light.vert"], roll["shader/dynamic_light.frag"]));
	uniform_view_projection = get_uniform(program, "view_projection");

	glBindVertexArray(vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, vbo.get());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

void GLDynamicLightRenderer::set_view_projection(const glm::mat4 &vp)
{
	glUseProgram(program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(vp));
}

void GLDynamicLightRenderer::render(const float *vertices, int count)
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, vbo.get());

	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_DYNAMIC_DRAW);

	if (count % 2 != 0)
		win::bug("GLDynamicLightRenderer: nonsensical vertex count");

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, count / 2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

void GLDynamicLightRenderer::flush()
{}

