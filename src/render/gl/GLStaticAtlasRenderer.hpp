#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Renderable.hpp"
#include "AtlasTextureCollection.hpp"
#include "GLSubRenderer.hpp"

class GLStaticAtlasRenderer : public GLSubRenderer
{
public:
	GLStaticAtlasRenderer(win::AssetRoll &roll, const TextureMap &texture_map, AtlasTextureCollection &atlas_textures);

	void set_view_projection(const glm::mat4 &view_projection);
	std::uint16_t load(const Renderable &items);
	void finalize();

	void add(std::uint16_t base_vertex);
	void flush() override;

private:
	win::GLProgram program;
	int uniform_view_projection;

	const TextureMap &texture_map;
	const AtlasTextureCollection &atlas_textures;

	win::GLVertexArray vao;
	win::GLBuffer position, texcoord, index;

	std::vector<std::uint16_t> scene;

	struct LoadStaging
	{
		LoadStaging() { count = 0; }

		std::vector<float> position;
		std::vector<std::uint16_t> texcoord;
		std::vector<std::uint16_t> index;
		int count;
	} staging;
};
