#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/gl/GL.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GLAtlas.hpp>

#include "../../Darktimes.hpp"
#include "../../Renderable.hpp"


class DynamicPass
{
	NO_COPY_MOVE(DynamicPass);

	static constexpr int ubo_size = 2;

public:
	explicit DynamicPass(win::AssetRoll&);
	~DynamicPass();

	void set_projection(const glm::mat4&) const;
	void set_view(const glm::mat4&) const;

	void set_renderables(const std::vector<Renderable>&);
	void draw();

private:
	GLuint program;
	GLint uniform_projection;
	GLint uniform_view;

	GLuint vao;
	GLuint vbo_vertices;
	GLuint vbo_drawids;
	GLuint ebo;
	GLuint ubo_transforms;

	win::GLAtlas atlas;
};
