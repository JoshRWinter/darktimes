#pragma once

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Renderable.hpp"
#include "FloorTextureCollection.hpp"
#include "GLSubRenderer.hpp"

class GLStaticFloorRenderer : public GLSubRenderer
{
public:
	GLStaticFloorRenderer(win::AssetRoll &roll, const FloorTextureCollection &floortextures);

	void set_view_projection(const glm::mat4 &view);
	std::uint16_t load(const Renderable &floor);
	void finalize();
	void add(std::uint16_t item);
	void flush() override;

private:
    const FloorTextureCollection &floortextures;
	win::GLProgram program;
	int uniform_view_projection;

	win::GLVertexArray vao;
	win::GLBuffer position_texcoord, layer, index;

	std::vector<std::uint16_t> scene;

	struct LoadStaging
	{
		LoadStaging() { count = 0; }

		std::vector<float> position_texcoord;
		std::vector<std::uint8_t> layer;
		std::vector<std::uint16_t> index;
		int count;
	} staging;
};
