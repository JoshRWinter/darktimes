#pragma once

#include <tuple>

#include <glm/glm.hpp>

#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/gl/GLTextRenderer.hpp>
#include <win/AssetRoll.hpp>
#include <win/Pool.hpp>

#include "../RendererBackend.hpp"

enum class GLResourceType { floor, atlas };
struct GLResource
{
	GLResourceType type;
	int vbo_offset;
	union
	{
		float floor_layer;
		const win::GLAtlas *atlas;
	} properties;
};

class GLRendererBackend : public RendererBackend
{
	WIN_NO_COPY_MOVE(GLRendererBackend);

public:
	GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll);

	const win::Font &create_font(win::Stream data, float size) override;
	void draw_text(const win::Font &font, const char *text, float x, float y, bool centered) override;
	void set_view(float x, float y, float zoom) override;
	void begin() override;
	void end() override;
	void render(const std::vector<Renderable> &objects) override;

private:
	std::tuple<std::vector<float>, std::vector<std::uint16_t>, std::vector<std::uint8_t>> load_resources(win::AssetRoll &roll);
	win::GLAtlas &get_atlas(int index);
	static void generate_vertices(std::vector<float> &pos, std::vector<std::uint16_t> &tc, std::vector<std::uint8_t> &indices, float left, float right, float bottom, float top);

	win::Pool<win::GLAtlas> atlases;
	std::vector<GLResource> resources;

	struct
	{
		struct
		{
			win::GLProgram program;
			int uniform_view_projection;
			int uniform_position;
			int uniform_size;
		} atlas;

		struct
		{
			win::GLProgram program;
			int uniform_view_projection;
			int uniform_position;
			int uniform_size;
			int uniform_layer;
			int uniform_tc_scale;
			win::GLTexture tex;
		} floor;
	} mode;

	glm::mat4 projection;

	win::GLVertexArray vao;
	win::GLBuffer vbo_positions, vbo_texcoords, vbo_indices;

	win::GLTextRenderer text_renderer;
	win::Pool<win::GLFont> fonts;
};
