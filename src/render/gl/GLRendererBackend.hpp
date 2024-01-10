#pragma once

#include <glm/glm.hpp>

#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/gl/GLTextRenderer.hpp>
#include <win/AssetRoll.hpp>
#include <win/Pool.hpp>

#include "GLFloorTextureCollection.hpp"
#include "GLAtlasTextureCollection.hpp"

#include "GLStaticFloorRenderer.hpp"
#include "GLStaticAtlasRenderer.hpp"
#include "GLDynamicAtlasRenderer.hpp"
#include "../RendererBackend.hpp"

enum class GLLoadedObjectType : std::uint16_t
{
	floor,
	atlas
};

struct GLLoadedObject
{
	GLLoadedObject(GLLoadedObjectType type, std::uint16_t base_vertex) : type(type), base_vertex(base_vertex) {}

	GLLoadedObjectType type;
	std::uint16_t base_vertex;
};

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
	void load_dynamics() override;
	void render_start() override;
	void render_end() override;
	void render_statics(const std::vector<const void*> &statics) override;
	void render_dynamics(const std::vector<Renderable> &dynamics) override;

private:
	TextureAssetMap texture_map;
	GLFloorTextureCollection floor_textures;
	GLAtlasTextureCollection atlases;

	GLStaticFloorRenderer static_floor_renderer;
	GLStaticAtlasRenderer static_atlas_renderer;
	GLDynamicAtlasRenderer dynamic_atlas_renderer;
	std::vector<GLLoadedObject> loaded_statics;

	glm::mat4 projection;

	win::GLTextRenderer text_renderer;
	win::Pool<win::GLFont> fonts;
};
