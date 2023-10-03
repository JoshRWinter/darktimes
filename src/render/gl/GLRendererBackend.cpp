#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLRendererBackend.hpp"
#include "../../TextureDefinitions.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: static_floor_renderer(roll)
	, text_renderer(screen_dims, projection, GL_TEXTURE1, true)
{
	fprintf(stderr, "%s\n%s\n%s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

	/*
	 * AYYYYYYYYYYYYY
	 * limit index depth
	 * limit atlas count
	 * fix 32bit bullshit
	 */

	this->projection = glm::ortho(projection.left, projection.right, projection.bottom, projection.top);

	glClearColor(0.4f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	floor_texture = load_floor_textures(roll);
	atlases = load_atlases(roll);
}

const win::Font &GLRendererBackend::create_font(win::Stream data, float size)
{
	glActiveTexture(GL_TEXTURE15);
	return fonts.add(std::move(text_renderer.create_font(size, std::move(data))));
}

void GLRendererBackend::draw_text(const win::Font &font, const char *text, float x, float y, bool centered)
{
	text_renderer.draw((win::GLFont&)font, text, x, y, centered);
	text_renderer.flush();
}

void GLRendererBackend::set_view(float x, float y, float zoom)
{
	const auto ident = glm::identity<glm::mat4>();
	const auto translate = glm::translate(ident, glm::vec3(-x, -y, 0.0f));
	const auto scale = glm::scale(ident, glm::vec3(zoom, zoom, 0.0f));
	const auto view = scale * translate;
	const auto vp = projection * view;

	static_floor_renderer.set_view_projection(vp);
}

std::vector<const void*> GLRendererBackend::load_statics(const std::vector<Renderable> &statics)
{
	std::vector<Renderable> floors;
	for (const auto &r : statics)
		if (TextureDefinitions.textures[r.texture].atlas == -1)
			floors.push_back(r);

	return static_floor_renderer.load_static_floors(floors, floor_layer_map);
}

void GLRendererBackend::render(const std::vector<const void*> &objects)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (const auto o : objects)
		static_floor_renderer.add(o);

	static_floor_renderer.flush();

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

std::vector<win::GLAtlas> GLRendererBackend::load_atlases(win::AssetRoll &roll)
{
	glActiveTexture(GL_TEXTURE0);
	std::vector<win::GLAtlas> result;
	for (const char *name : TextureDefinitions.atlases)
		result.emplace_back(roll[name], win::GLAtlas::Mode::linear);

	return result;
}

win::GLTexture GLRendererBackend::load_floor_textures(win::AssetRoll &roll)
{
	int width = -1, height = -1;

	win::GLTexture tex;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex.get());
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 0 initialize a bunch of entries in the map
	floor_layer_map.resize(TextureDefinitions.textures.size());

	int index = 0;
	for (const auto i : TextureDefinitions.floor_textures)
	{
		win::Targa tga(roll[TextureDefinitions.textures[i].resource]);
		floor_layer_map[i] = index;

		if (width == -1)
		{
			width = tga.width();
			height = tga.height();
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, TextureDefinitions.floor_textures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		if (width != tga.width() || height != tga.height())
			win::bug("All floor textures must be same dimensions!");

		if (tga.bpp() != 24 && false)
			win::bug("Prefer 24 bit color for floor textures");

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tga.data());
		++index;
	}

	return tex;
}
