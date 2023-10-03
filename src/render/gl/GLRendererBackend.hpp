#pragma once

#include <glm/glm.hpp>

#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/gl/GLTextRenderer.hpp>
#include <win/AssetRoll.hpp>
#include <win/Pool.hpp>

#include "FloorTextureCollection.hpp"
#include "AtlasTextureCollection.hpp"

#include "GLStaticFloorRenderer.hpp"
#include "GLStaticAtlasRenderer.hpp"
#include "../RendererBackend.hpp"

class GLRendererBackend : public RendererBackend
{
	WIN_NO_COPY_MOVE(GLRendererBackend);

public:
	GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll);
	~GLRendererBackend() override = default;

	const win::Font &create_font(win::Stream data, float size) override;
	void draw_text(const win::Font &font, const char *text, float x, float y, bool centered) override;
	void set_view(float x, float y, float zoom) override;
	std::vector<const void*> load_statics(const std::vector<Renderable> &statics) override;
	void render_statics(const std::vector<const void*> &statics) override;

private:
	FloorTextureCollection floor_textures;
	//AtlasTextureCollection atlases;

	GLStaticFloorRenderer static_floor_renderer;
	//GLStaticAtlasRenderer static_atlas_renderer;
	std::vector<std::uint16_t> loaded_statics;

	glm::mat4 projection;

	win::GLTextRenderer text_renderer;
	win::Pool<win::GLFont> fonts;
};
