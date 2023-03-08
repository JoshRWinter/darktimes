#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <win/gl/GL.hpp>

#include "GLRenderer.hpp"

GLRenderer::GLRenderer(const win::Area<float> &world_area, win::AssetRoll &roll)
	: floor_pass(roll)
	, prop_pass(roll)
	, dynamic_pass(roll)
{
	const glm::mat4 projection_matrix = glm::ortho(world_area.left, world_area.right, world_area.bottom, world_area.top);

	floor_pass.set_projection(projection_matrix);
	prop_pass.set_projection(projection_matrix);
	dynamic_pass.set_projection(projection_matrix);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLRenderer::set_level_data(const std::vector<Renderable> &tile_renderables, const std::vector<Renderable> &atlas_renderables)
{
	floor_pass.set_floors(tile_renderables);
	prop_pass.set_props(atlas_renderables);
}

void GLRenderer::set_dynamics(const std::vector<Renderable> &dynamics)
{
	dynamic_pass.set_renderables(dynamics);
}

void GLRenderer::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	floor_pass.draw();
	prop_pass.draw();
	dynamic_pass.draw();

#ifndef NDEBUG
	auto error = glGetError();
	if (error != 0)
		win::bug("GL error " + std::to_string(error));
#endif
}

void GLRenderer::set_center(float x, float y)
{
	const float zoom = 0.5f;
	const glm::mat4 view_matrix = glm::translate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(zoom, zoom, zoom)), glm::vec3(-x, -y, 0.0f));

	floor_pass.set_view(view_matrix);
	prop_pass.set_view(view_matrix);
	dynamic_pass.set_view(view_matrix);
}
