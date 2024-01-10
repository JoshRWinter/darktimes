#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Renderable.hpp"
#include "AtlasTextureCollection.hpp"
#include "GLSubRenderer.hpp"

class GLDynamicAtlasRenderer : public GLSubRenderer
{
public:
	GLDynamicAtlasRenderer(win::AssetRoll &roll, const TextureAssetMap &texture_map, const AtlasTextureCollection &atlas_textures);

	void set_view_projection(const glm::mat4 &view_projection);
	void load_all();
	void add(const Renderable &renderable);
	void flush() override;

private:
	win::GLProgram program;
	int uniform_view_projection;
	int uniform_position;
	int uniform_size;
	int uniform_rotation;

	const TextureAssetMap &texture_map;
	const AtlasTextureCollection &atlas_textures;
	std::uint16_t base_vertex_map[(int)Texture::max_texture];

	win::GLVertexArray vao;
	win::GLBuffer position, texcoord, index;

	std::vector<const Renderable*> scene;
};
