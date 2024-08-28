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
	explicit GLDynamicLightRenderer(win::AssetRoll &roll);

	void set_view_projection(const glm::mat4 &vp);
	void set_viewport(const win::Dimensions<int> &viewport);
	void render(const float *vertices, int count, GLuint fbo);
	void flush() override;

private:
	win::Dimensions<int> viewport;

	struct
	{
		win::GLProgram program;
		win::GLFramebuffer fbo;
		win::GLTexture fbo_tex;
		win::GLVertexArray vao;
		win::GLBuffer vbo;
		int uniform_view_projection;
	} light;

	struct
	{
		win::GLProgram program;
		win::GLFramebuffer fbo;
		win::GLTexture fbo_tex;
		win::GLVertexArray vao;
		win::GLBuffer vbo;
		int uniform_horizontal;
	} blur;
};
