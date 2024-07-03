#pragma once

#include <glm/glm.hpp>

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "GLSubRenderer.hpp"

class GLDynamicLightRenderer : public GLSubRenderer
{
	WIN_NO_COPY_MOVE(GLDynamicLightRenderer);

public:
	explicit GLDynamicLightRenderer(win::AssetRoll &roll);

	void set_view_projection(const glm::mat4 &vp);
	void render(const float *vertices, int count);
	void flush() override;

private:
	win::GLProgram light_program;
	int uniform_view_projection;

	win::GLProgram overlay_program;
	int uniform_overlay_sampler;

	win::GLVertexArray light_vao;
	win::GLBuffer light_vbo;

	win::GLVertexArray overlay_vao;
	win::GLBuffer overlay_vbo;

	win::GLFramebuffer fbo;
	win::GLTexture light_texture;
};
