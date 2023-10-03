#pragma once

#include <glm/glm.hpp>

#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/gl/GLTextRenderer.hpp>
#include <win/AssetRoll.hpp>
#include <win/Pool.hpp>

#include "GLStaticFloorRenderer.hpp"
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
	std::vector<const void*> load_statics(const std::vector<Renderable> &renderables) override;
	void render(const std::vector<const void*> &objects) override;

private:
	static std::vector<win::GLAtlas> load_atlases(win::AssetRoll &roll);
	win::GLTexture load_floor_textures(win::AssetRoll &roll);

	win::GLTexture floor_texture;
	std::vector<win::GLAtlas> atlases;
	std::vector<int> floor_layer_map;
	std::vector<int> atlas_item_map;
	GLStaticFloorRenderer static_floor_renderer;

	glm::mat4 projection;

	win::GLTextRenderer text_renderer;
	win::Pool<win::GLFont> fonts;
};
