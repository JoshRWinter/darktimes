#include <chrono>

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
	//light.uniform.viewport_width = get_uniform(light.program, "viewport_width");
	light.uniform.power = get_uniform(light.program, "power");
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, viewport.width, viewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, blur.fbo_tex.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, viewport.width, viewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void GLDynamicLightRenderer::set_geometry(const std::vector<Line> &lines)
{
	this->lines = lines;
}

void GLDynamicLightRenderer::render(float x, float y, float power, const win::Color<float> &rgb)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, light.fbo.get());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(light.program.get());
	glBindVertexArray(light.vao.get());

	const float cutoff_threshold = 0.2f;
	const auto radius = std::sqrtf((power - cutoff_threshold) / cutoff_threshold);

	const auto screen_xy = world_to_screen(glm::vec2(x, y), projection * view);
	glUniform2i(light.uniform.light, screen_xy.x, screen_xy.y);
	glUniform3f(light.uniform.lightcolor, 1.0f, 1.0f, 1.0f);
	//glUniform1i(light.uniform.viewport_width,
	glUniform1f(light.uniform.power, power);

	const glm::vec4 world_topright = glm::inverse(view) * glm::inverse(projection) * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	const glm::vec4 world_bottomleft = glm::inverse(view) * glm::inverse(projection) * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);

	struct Box
	{
		Box(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
		float x1, y1, x2, y2;
	};

	const Box light_geometry(x - radius, y - radius, x + radius,  y + radius);
	const Box screen_geometry(world_bottomleft.x, world_bottomleft.y, world_topright.x, world_topright.y);
	const Box overlap(std::max(light_geometry.x1, screen_geometry.x1), std::max(light_geometry.y1, screen_geometry.y1), std::min(light_geometry.x2, screen_geometry.x2), std::min(light_geometry.y2, screen_geometry.y2));

	// build the line data for the shader
	const win::Box<float> quadrants[]
	{
		win::Box<float>(overlap.x1, overlap.y1, x - overlap.x1, y - overlap.y1), // lower left
		win::Box<float>(x, overlap.y1, overlap.x2 - x, y - overlap.y1), // lower right
		win::Box<float>(overlap.x1, y, x - overlap.x1, overlap.y2 - y), // upper left
		win::Box<float>(x, y, overlap.x2 - x, overlap.y2 - y) // upper right
	};

	int offsets[4];
	int lengths[4];

	std::vector<int> data;

	const auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 4; ++i)
	{
		offsets[i] = data.size();

		for (const auto &line : lines)
		{
			const auto minbox = boundingbox(line);

			if (overlaps(minbox, quadrants[i]))
			{
				const auto a = world_to_screen(glm::vec4(line.x1, line.y1, 0.0f, 1.0f), projection * view);
				const auto b = world_to_screen(glm::vec4(line.x2, line.y2, 0.0f, 1.0f), projection * view);

				data.push_back(a.x);
				data.push_back(a.y);
				data.push_back(b.x);
				data.push_back(b.y);
			}
		}

		lengths[i] = data.size() - offsets[i];
	}
	const auto stop = std::chrono::high_resolution_clock::now();
	const auto millis = std::chrono::duration<float, std::milli>(stop - start).count();
	fprintf(stderr, "Built %lu lines in %.1f milliseconds\n", data.size() / 4, millis);

	// upload line data
	glBindBuffer(GL_UNIFORM_BUFFER, light.linedata.get());
	glBufferData(GL_UNIFORM_BUFFER, sizeof(int) * data.size(), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, light.linedata.get());

	for (int i = 0; i < 4; ++i)
	{
		glUniform2i(light.uniform.range, offsets[i] / 4, lengths[i] / 4);

		// position the light geometry
		const auto ident = glm::identity<glm::mat4>();
		const auto translate = glm::translate(ident, glm::vec3(quadrants[i].x + (quadrants[i].width / 2.0f), quadrants[i].y + (quadrants[i].height / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(quadrants[i].width, quadrants[i].height, 1.0f));
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
	return glm::ivec2(std::roundf(((clip.x + 1.0f) / 2.0f) * viewport.width), std::roundf(((clip.y + 1.0f) / 2.0f) * viewport.height));
}

win::Box<float> GLDynamicLightRenderer::boundingbox(const Line &line)
{
	const float left = std::min(line.x1, line.x2);
	const float right = std::max(line.x1, line.x2);
	const float bottom = std::min(line.y1, line.y2);
	const float top = std::max(line.y1, line.y2);

	return win::Box<float>(left, bottom, right - left, top - bottom);
}

bool GLDynamicLightRenderer::overlaps(const win::Box<float> &a, const win::Box<float> &b)
{
	return a.x + a.width > b.x && a.x < b.x + b.width && a.y + a.height > b.y && a.y < b.y + b.height;
}
