#pragma once

#include <glm/glm.hpp>

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/Utility.hpp>

#include "GLSubRenderer.hpp"

class GLDynamicLightRenderer : public GLSubRenderer
{
	WIN_NO_COPY_MOVE(GLDynamicLightRenderer);

public:
	struct Line
	{
		Line(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
		float x1, y1, x2, y2;
	};

	explicit GLDynamicLightRenderer(win::AssetRoll &roll);

	void set_view_projection(const glm::mat4 &view, const glm::mat4 &projection);
	void set_viewport(const win::Dimensions<int> &viewport);
	void set_geometry(const std::vector<Line> &lines);
	void render(float x, float y, float radius, const win::Color<float> &rgb);
	void flush() override;

private:
	glm::ivec2 world_to_screen(const glm::vec2 &vert, const glm::mat4 &transform) const;
	static win::Box<float> boundingbox(const Line &line);
	static bool overlaps(const win::Box<float> &a, const win::Box<float> &b);

	win::Dimensions<int> viewport;
	glm::mat4 view;
	glm::mat4 projection;
	std::vector<Line> lines;

	struct
	{
		win::GLProgram program;
		win::GLFramebuffer fbo;
		win::GLTexture fbo_tex;
		win::GLVertexArray vao;
		win::GLBuffer linedata;

		struct
		{
			GLint mvp;
			GLint light;
			GLint power;
			GLint viewport_width;
			GLint lightcolor;
			GLint range;
		} uniform;
	} light;

	struct
	{
		win::GLProgram program;
		win::GLFramebuffer fbo;
		win::GLTexture fbo_tex;
		win::GLVertexArray vao;
		win::GLBuffer vbo;
		struct
		{
			GLint horizontal;
		} uniform;
	} blur;
};
