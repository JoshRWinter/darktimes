#pragma once

/*
#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>

#include "../../Renderable.hpp"
#include "AtlasTextureCollection.hpp"
#include "GLSubRenderer.hpp"

class GLDynamicAtlasRenderer : public GLSubRenderer
{
public:
	explicit GLDynamicAtlasRenderer(win::AssetRoll &roll);

	void set_atlases(const std::vector<win::GLAtlas> &atlases);
	void set_view_projection(const glm::mat4 &view_projection);
	std::uint16_t load(const Renderable &renderable);
	void add(const Renderable &renderable, std::uint32_t base_vertex);
	void flush() override;

private:
	win::GLProgram program;
	int uniform_view_projection;
	int uniform_position;
	int uniform_size;
	int uniform_rotation;

	win::GLVertexArray vao;
	win::GLBuffer position, texcoord, index;

	const std::vector<win::GLAtlas> *atlases;
	GLuint current_texture;

	std::vector<GLDynamicAtlasSceneItem> scene;
};
 */
