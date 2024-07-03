#include "glm/gtc/type_ptr.hpp"

#include "GLDynamicLightRenderer.hpp"

using namespace win::gl;

GLDynamicLightRenderer::GLDynamicLightRenderer(win::AssetRoll &roll)
{
	light_program = win::GLProgram(win::load_gl_shaders(roll["shader/dynamic_light.vert"], roll["shader/dynamic_light.frag"]));
	uniform_view_projection = get_uniform(light_program, "view_projection");

	overlay_program = win::GLProgram(win::load_gl_shaders(roll["shader/overlay.vert"], roll["shader/overlay.frag"]));
	uniform_overlay_sampler = get_uniform(overlay_program, "tex");

	glUseProgram(overlay_program.get());
	glUniform1i(uniform_overlay_sampler, 2);

	glBindVertexArray(light_vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, light_vbo.get());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(overlay_vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, overlay_vbo.get());

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	const float overlay_verts[] =
	{
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_verts), overlay_verts, GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, light_texture.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo.get());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, light_texture.get(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	check_error();
}

void GLDynamicLightRenderer::set_view_projection(const glm::mat4 &vp)
{
	glUseProgram(light_program.get());
	glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(vp));
}

void GLDynamicLightRenderer::render(const float *vertices, int count)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo.get());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(light_program.get());
	glBindVertexArray(light_vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, light_vbo.get());

	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_DYNAMIC_DRAW);

	if (count % 2 != 0)
		win::bug("GLDynamicLightRenderer: nonsensical vertex count");

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, count / 2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(overlay_program.get());
	glBindVertexArray(overlay_vao.get());
	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	check_error();
}

void GLDynamicLightRenderer::flush()
{}

