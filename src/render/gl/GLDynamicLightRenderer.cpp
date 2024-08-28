#include "glm/gtc/type_ptr.hpp"

#include "GLDynamicLightRenderer.hpp"

using namespace win::gl;

GLDynamicLightRenderer::GLDynamicLightRenderer(win::AssetRoll &roll)
{
	glActiveTexture(GL_TEXTURE2);

	// light mode setup

	glBindTexture(GL_TEXTURE_2D, light.fbo_tex.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, light.fbo.get());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, light.fbo_tex.get(), 0);

	light.program = win::GLProgram(win::load_gl_shaders(roll["shader/dynamic_light.vert"], roll["shader/dynamic_light.frag"]));
	light.uniform_view_projection = get_uniform(light.program, "view_projection");

	glBindVertexArray(light.vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, light.vbo.get());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// blur mode setup

	glBindTexture(GL_TEXTURE_2D, blur.fbo_tex.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur.fbo_tex.get(), 0);

	blur.program = win::GLProgram(win::load_gl_shaders(roll["shader/blur.vert"], roll["shader/blur.frag"]));
	glUseProgram(blur.program.get());

	blur.uniform_horizontal = get_uniform(blur.program, "horizontal");
	glUniform1i(get_uniform(blur.program, "tex"), 2);

	glBindVertexArray(blur.vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, blur.vbo.get());

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

	// reset fbo

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	check_error();
}

void GLDynamicLightRenderer::set_view_projection(const glm::mat4 &vp)
{
	glUseProgram(light.program.get());
	glUniformMatrix4fv(light.uniform_view_projection, 1, GL_FALSE, glm::value_ptr(vp));
}

void GLDynamicLightRenderer::set_viewport(const win::Dimensions<int> &viewport)
{
	this->viewport = viewport;

	glActiveTexture(GL_TEXTURE2);

	glBindTexture(GL_TEXTURE_2D, light.fbo_tex.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, viewport.width, viewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, blur.fbo_tex.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, viewport.width, viewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void GLDynamicLightRenderer::render(const float *vertices, int count)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, light.fbo.get());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(light.program.get());
	glBindVertexArray(light.vao.get());
	glBindBuffer(GL_ARRAY_BUFFER, light.vbo.get());

	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_DYNAMIC_DRAW);

	if (count % 2 != 0)
		win::bug("GLDynamicLightRenderer: nonsensical vertex count");

	// draw the light mesh
	glDrawArrays(GL_TRIANGLE_FAN, 0, count / 2);

	// blur
	glUseProgram(blur.program.get());
	glBindVertexArray(blur.vao.get());

	glActiveTexture(GL_TEXTURE2);

	glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
	glBindTexture(GL_TEXTURE_2D, light.fbo_tex.get());
	glUniform1i(blur.uniform_horizontal, 1);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, blur.fbo_tex.get());
	glUniform1i(blur.uniform_horizontal, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	check_error();
}

void GLDynamicLightRenderer::flush()
{}

