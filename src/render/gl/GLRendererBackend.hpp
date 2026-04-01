#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/gl/GL.hpp>
#include <win/gl/GLTextRenderer.hpp>
#include <win/AssetRoll.hpp>

#include "GLFloorRenderer.hpp"
#include "GLAtlasRenderer.hpp"
#include "../RendererBackend.hpp"
#include "../TextureAssetMap.hpp"

struct StaticObject
{
	enum class Type
	{
		floor,
		atlas
	};

	StaticObject(Type type, std::uint16_t base_vertex) : type(type), base_vertex(base_vertex) {}

	Type type;
	std::uint16_t base_vertex;
};

class GLRendererBackend : public RendererBackend
{
	WIN_NO_COPY_MOVE(GLRendererBackend);

public:
	GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll);
	~GLRendererBackend() override = default;

	void set_view(float x, float y, float zoom) override;
	void load_statics(const std::vector<Renderable> &statics) override;
	void render_statics(const std::vector<int> &statics) override;
	void render_dynamics(const std::vector<Renderable> &dynamics) override;

private:
	static void check_error();

	glm::mat4 projection;

	TextureAssetMap texture_map;

	GLFloorRenderer floor_renderer;
	GLAtlasRenderer atlas_renderer;
	std::vector<StaticObject> loaded_statics;

	win::GLTextRenderer text_renderer;

	std::vector<std::uint16_t> renderable_ids;
};
