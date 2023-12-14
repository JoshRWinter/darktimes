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

enum class GLLoadedObjectType
{
	static_floor,
	static_atlas
};

struct GLLoadedObject
{
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
	std::vector<const void*> load_statics(const std::vector<Renderable> &renderables) override;
	void render(const std::vector<const void*> &objects) override;

private:
	static std::vector<win::GLAtlas> load_atlases(win::AssetRoll &roll);
	win::GLTexture load_floor_textures(win::AssetRoll &roll);

	FloorTextureCollection floor_textures;
	AtlasTextureCollection atlases;

	GLStaticFloorRenderer static_floor_renderer;
	GLStaticAtlasRenderer static_atlas_renderer;
	std::vector<GLLoadedObject> loaded_objects;
	std::vector<Renderable> scratch;

	glm::mat4 projection;

	win::GLTextRenderer text_renderer;
	win::Pool<win::GLFont> fonts;
};
