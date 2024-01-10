#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Renderable.hpp"
#include "GLAtlasTextureCollection.hpp"
#include "GLSubRenderer.hpp"

class GLStaticAtlasRenderer : public GLSubRenderer
{
public:
	explicit GLStaticAtlasRenderer(win::AssetRoll &roll);

	void set_view_projection(const glm::mat4 &view_projection);
	std::vector<std::uint16_t> load(const std::vector<Renderable> &renderables, const GLAtlasTextureCollection &atlases, const TextureAssetMap &texture_map);

	void add(std::uint16_t base_vertex);
	void flush() override;

private:
	win::GLProgram program;
	int uniform_view_projection;

	win::GLVertexArray vao;
	win::GLBuffer position, texcoord, index;

	std::vector<std::uint16_t> scene;
};
