#pragma once

#include <vector>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Renderable.hpp"
#include "GLFloorTextureCollection.hpp"
#include "GLSubRenderer.hpp"

class GLStaticFloorRenderer : public GLSubRenderer
{
public:
	explicit GLStaticFloorRenderer(win::AssetRoll &roll);

	void set_view_projection(const glm::mat4 &view);
	std::vector<std::uint16_t> load(const std::vector<Renderable> &renderables, const GLFloorTextureCollection &floor_textures);
	void add(std::uint16_t item);
	void flush() override;

private:
	win::GLProgram program;
	int uniform_view_projection;

	win::GLVertexArray vao;
	win::GLBuffer position_texcoord, layer, index;

	std::vector<std::uint16_t> scene;
};
