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
	light.uniform.range = get_uniform(light.program, "range");

	glUniform2i(light.uniform.range, 0, 0);

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

void GLDynamicLightRenderer::set_view_projection(const glm::mat4 &view, const glm::mat4 &projection)
{
	this->view = view;
	this->projection = projection;
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
	this->lines.clear();
	for (const auto &line : lines)
	{
		const auto a = world_to_screen(glm::vec2(line.x1, line.y1), projection);
		const auto b = world_to_screen(glm::vec2(line.x2, line.y2), projection);

		this->lines.emplace_back(a.x, a.y, b.x, b.y);
	}
}

void GLDynamicLightRenderer::render(float x, float y, float radius, const win::Color<float> &rgb)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, light.fbo.get());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(light.program.get());
	glBindVertexArray(light.vao.get());

	// screen-space representations of the light position and radius
	const auto screen_lightpos = world_to_screen(glm::vec2(x, y), projection * view);
	const auto screen_radius = world_to_screen_size(glm::vec2(radius, 0.0f), projection).x;

	glUniform2i(light.uniform.light, screen_lightpos.x, screen_lightpos.y);
	glUniform3f(light.uniform.lightcolor, 1.0f, 1.0f, 1.0f);
	//glUniform1i(light.uniform.radius, 500);

	// build the line data for the shader
	const auto screen_space_reference = world_to_screen(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), projection);
	const auto screen_space_view_shift = world_to_screen(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), projection * view);
	const glm::ivec2 screen_space_line_shift(screen_space_view_shift.x - screen_space_reference.x, screen_space_view_shift.y - screen_space_reference.y);

	const win::Box<int> screen_upperleft(screen_lightpos.x - screen_radius, screen_lightpos.y, screen_radius, screen_radius);
	const win::Box<int> screen_upperright(screen_lightpos.x, screen_lightpos.y, screen_radius, screen_radius);
	const win::Box<int> screen_lowerleft(screen_lightpos.x - screen_radius, screen_lightpos.y - screen_radius, screen_radius, screen_radius);
	const win::Box<int> screen_lowerright(screen_lightpos.x, screen_lightpos.y - screen_radius, screen_radius, screen_radius);

	const glm::vec2 world_upperleft(x - radius, y);
	const glm::vec2 world_upperright(x, y);
	const glm::vec2 world_lowerleft(x - radius, y - radius);
	const glm::vec2 world_lowerright(x, y - radius);

	std::vector<int> data;

	struct OffsetLength { OffsetLength(int offset, int length) : offset(offset), length(length) {} int offset, length; };
	std::vector<OffsetLength> offsets;

	for (const auto &line : lines)
	{
		const InternalLine translated(line.x1 + screen_space_line_shift.x, line.y1 + screen_space_line_shift.y, line.x2 + screen_space_line_shift.x, line.y2 + screen_space_line_shift.y);
		const auto box = boundingbox(translated);

		if (overlaps(box, screen_upperleft))
		{
			data.push_back(line.x1 + screen_space_line_shift.x);
			data.push_back(line.y1 + screen_space_line_shift.y);
			data.push_back(line.x2 + screen_space_line_shift.x);
			data.push_back(line.y2 + screen_space_line_shift.y);
		}
	}
	const int upperleft_offset = 0;
	const int upperleft_length = data.size();

	for (const auto &line : lines)
	{
		const InternalLine translated(line.x1 + screen_space_line_shift.x, line.y1 + screen_space_line_shift.y, line.x2 + screen_space_line_shift.x, line.y2 + screen_space_line_shift.y);
		const auto box = boundingbox(translated);

		if (overlaps(box, screen_upperright))
		{
			data.push_back(line.x1 + screen_space_line_shift.x);
			data.push_back(line.y1 + screen_space_line_shift.y);
			data.push_back(line.x2 + screen_space_line_shift.x);
			data.push_back(line.y2 + screen_space_line_shift.y);
		}
	}
	const int upperright_offset = upperleft_length;
	const int upperright_length = data.size() - upperright_offset;

	for (const auto &line : lines)
	{
		const InternalLine translated(line.x1 + screen_space_line_shift.x, line.y1 + screen_space_line_shift.y, line.x2 + screen_space_line_shift.x, line.y2 + screen_space_line_shift.y);
		const auto box = boundingbox(translated);

		if (overlaps(box, screen_lowerleft))
		{
			data.push_back(line.x1 + screen_space_line_shift.x);
			data.push_back(line.y1 + screen_space_line_shift.y);
			data.push_back(line.x2 + screen_space_line_shift.x);
			data.push_back(line.y2 + screen_space_line_shift.y);
		}
	}
	const int lowerleft_offset = upperright_offset + upperright_length;
	const int lowerleft_length = data.size() - lowerleft_offset;

	for (const auto &line : lines)
	{
		const InternalLine translated(line.x1 + screen_space_line_shift.x, line.y1 + screen_space_line_shift.y, line.x2 + screen_space_line_shift.x, line.y2 + screen_space_line_shift.y);
		const auto box = boundingbox(translated);

		if (overlaps(box, screen_lowerright))
		{
			data.push_back(line.x1 + screen_space_line_shift.x);
			data.push_back(line.y1 + screen_space_line_shift.y);
			data.push_back(line.x2 + screen_space_line_shift.x);
			data.push_back(line.y2 + screen_space_line_shift.y);
		}
	}
	const int lowerright_offset = lowerleft_offset + lowerleft_length;
	const int lowerright_length = data.size() - lowerright_offset;

	fprintf(stderr, "linecount: %lu\n", data.size() / 4);

	// upload line data
	glBindBuffer(GL_UNIFORM_BUFFER, light.linedata.get());
	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) * data.size(), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, light.linedata.get());

	{
		glUniform2i(light.uniform.range, 0, upperleft_length / 4);

		// position the light geometry
		const auto ident = glm::identity<glm::mat4>();
		const auto translate = glm::translate(ident, glm::vec3(world_upperleft.x + (radius / 2.0f), world_upperleft.y + (radius / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(radius, radius, 1.0f));
		const auto trans = projection * view * translate * scale;
		glUniformMatrix4fv(light.uniform.mvp, 1, GL_FALSE, glm::value_ptr(trans));

		// draw the light mesh
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	{
		glUniform2i(light.uniform.range, upperright_offset / 4, upperright_length / 4);

		// position the light geometry
		const auto ident = glm::identity<glm::mat4>();
		const auto translate = glm::translate(ident, glm::vec3(world_upperright.x + (radius / 2.0f), world_upperright.y + (radius / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(radius, radius, 1.0f));
		const auto trans = projection * view * translate * scale;
		glUniformMatrix4fv(light.uniform.mvp, 1, GL_FALSE, glm::value_ptr(trans));

		// draw the light mesh
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	{
		glUniform2i(light.uniform.range, lowerleft_offset / 4, lowerleft_length / 4);

		// position the light geometry
		const auto ident = glm::identity<glm::mat4>();
		const auto translate = glm::translate(ident, glm::vec3(world_lowerleft.x + (radius / 2.0f), world_lowerleft.y + (radius / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(radius, radius, 1.0f));
		const auto trans = projection * view * translate * scale;
		glUniformMatrix4fv(light.uniform.mvp, 1, GL_FALSE, glm::value_ptr(trans));

		// draw the light mesh
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	{
		glUniform2i(light.uniform.range, lowerright_offset / 4, lowerright_length / 4);

		// position the light geometry
		const auto ident = glm::identity<glm::mat4>();
		const auto translate = glm::translate(ident, glm::vec3(world_lowerright.x + (radius / 2.0f), world_lowerright.y + (radius / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(radius, radius, 1.0f));
		const auto trans = projection * view * translate * scale;
		glUniformMatrix4fv(light.uniform.mvp, 1, GL_FALSE, glm::value_ptr(trans));

		// draw the light mesh
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

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

glm::ivec2 GLDynamicLightRenderer::world_to_screen(const glm::vec2 &vert, const glm::mat4 &transform) const
{
	const glm::vec4 world(vert.x, vert.y, 0.0f, 1.0f);
	const auto clip = transform * world;
	return glm::ivec2(((clip.x + 1.0f) / 2.0f) * viewport.width, ((clip.y + 1.0f) / 2.0f) * viewport.height);
}

glm::ivec2 GLDynamicLightRenderer::world_to_screen_size(const glm::vec2 &size, const glm::mat4 &projection) const
{
	const auto reference = world_to_screen(glm::vec2(0.0f, 0.0f), projection);
	const auto point = world_to_screen(size, projection);

	return glm::ivec2(point.x - reference.x, point.y - reference.y);
}

win::Box<int> GLDynamicLightRenderer::boundingbox(const InternalLine &line)
{
	const int left = std::min(line.x1, line.x2);
	const int right = std::max(line.x1, line.x2);
	const int bottom = std::min(line.y1, line.y2);
	const int top = std::max(line.y1, line.y2);

	return win::Box<int>(left, bottom, right - left, top - bottom);
}

bool GLDynamicLightRenderer::overlaps(const win::Box<int> &a, const win::Box<int> &b)
{
	return a.x + a.width > b.x && a.x < b.x + b.width && a.y + a.height > b.y && a.y < b.y + b.height;
}
