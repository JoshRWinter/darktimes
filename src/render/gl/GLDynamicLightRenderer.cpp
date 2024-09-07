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
	glUseProgram(light.program.get());
	light.uniform.mvp = get_uniform(light.program, "mvp");
	light.uniform.light = get_uniform(light.program, "light");
	//light.uniform.radius = get_uniform(light.program, "radius");
	light.uniform.lightcolor = get_uniform(light.program, "lightcolor");
	light.uniform.linecount = get_uniform(light.program, "linecount");

	glUniform1i(light.uniform.linecount, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, light.linedata.get());

	{
		const auto loc = glGetUniformBlockIndex(light.program.get(), "LineData");
		if (loc == GL_INVALID_INDEX)
			win::bug("No LineData location");

		glUniformBlockBinding(light.program.get(), loc, 3);
	}

	// blur mode setup

	glBindTexture(GL_TEXTURE_2D, blur.fbo_tex.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur.fbo_tex.get(), 0);

	blur.program = win::GLProgram(win::load_gl_shaders(roll["shader/blur.vert"], roll["shader/blur.frag"]));
	glUseProgram(blur.program.get());

	blur.uniform.horizontal = get_uniform(blur.program, "horizontal");
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
	this->vp = vp;
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

void GLDynamicLightRenderer::set_geometry(const std::vector<Line> &lines)
{
	this->lines = lines;
}

void GLDynamicLightRenderer::render(float x, float y, float radius, const win::Color<float> &rgb)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, light.fbo.get());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(light.program.get());
	glBindVertexArray(light.vao.get());

	glUniform2i(light.uniform.light, 200, 200);
	//glUniform3f(light.uniform.lightcolor, 0.7f, 0.2f, 0.2f);
	glUniform3f(light.uniform.lightcolor, 1.0f, 1.0f, 1.0f);
	//glUniform1i(light.uniform.radius, 500);
	glUniform2i(light.uniform.light, screenspace(glm::vec2(x, y)).x, screenspace(glm::vec2(x, y)).y);

	{
		const auto ident = glm::identity<glm::mat4>();
		const auto translate = glm::translate(ident, glm::vec3(x, y, 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f));
		const auto trans = vp * translate * scale;
		glUniformMatrix4fv(light.uniform.mvp, 1, GL_FALSE, glm::value_ptr(trans));

		std::vector<int> data;
		for (const auto &line : lines)
		{
			auto box = boundingbox(line);

			if (box.x + box.width > x - radius && box.x < x + radius && box.y + box.height > y - radius && box.y < y + radius)
			{
				const auto a = screenspace(glm::vec2(line.x1, line.y1));
				const auto b = screenspace(glm::vec2(line.x2, line.y2));
				data.push_back(a.x);
				data.push_back(a.y);
				data.push_back(b.x);
				data.push_back(b.y);
			}
		}

		glUniform1i(light.uniform.linecount, data.size() / 4);
		fprintf(stderr, "linecount: %lu\n", data.size() / 4);

		glBindBuffer(GL_UNIFORM_BUFFER, light.linedata.get());
		glBufferData(GL_UNIFORM_BUFFER, sizeof(int) * data.size(), data.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, light.linedata.get());
	}

	// draw the light mesh
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// blur
	glUseProgram(blur.program.get());
	glBindVertexArray(blur.vao.get());

	glActiveTexture(GL_TEXTURE2);

	glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
	glBindTexture(GL_TEXTURE_2D, light.fbo_tex.get());
	glUniform1i(blur.uniform.horizontal, 1);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, blur.fbo_tex.get());
	glUniform1i(blur.uniform.horizontal, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	check_error();
}

void GLDynamicLightRenderer::flush()
{}

glm::vec2 GLDynamicLightRenderer::screenspace(const glm::vec2 &vert) const
{
	const glm::vec4 world(vert.x, vert.y, 0.0f, 1.0f);
	const auto clip = vp * world;
	const glm::vec2 screen(((clip.x + 1.0f) / 2.0f) * viewport.width, ((clip.y + 1.0f) / 2.0f) * viewport.height);

	return screen;
}

win::Box<float> GLDynamicLightRenderer::boundingbox(const GLDynamicLightRenderer::Line &line)
{
	const float left = std::min(line.x1, line.x2);
	const float right = std::max(line.x1, line.x2);
	const float bottom = std::min(line.y1, line.y2);
	const float top = std::max(line.y1, line.y2);

	return win::Box<float>(left, bottom, right - left, top - bottom);
}
