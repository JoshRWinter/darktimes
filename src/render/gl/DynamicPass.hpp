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

public:
	DynamicPass(win::AssetRoll&);
	~DynamicPass();

	void set_projection(const glm::mat4&);
	void set_view(const glm::mat4&);

	void set_renderables(const std::vector<Renderable>&);
	void draw();

private:
	GLuint program;
	GLint uniform_projection;
	GLint uniform_view;
	GLint uniform_model;

	GLuint vao;
	GLuint vbo;
	GLuint vbo_texcoords;
	GLuint ebo;

	win::GLAtlas atlas;
};
