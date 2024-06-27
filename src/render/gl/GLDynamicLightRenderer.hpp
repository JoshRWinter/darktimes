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
	win::GLProgram program;
	int uniform_view_projection;

	win::GLVertexArray vao;
	win::GLBuffer vbo;
};
