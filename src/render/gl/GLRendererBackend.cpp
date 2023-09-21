#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLRendererBackend.hpp"
#include "../../TextureDefinitions.hpp"

using namespace win::gl;

static int get_uniform(const win::GLProgram &p, const char *un)
{
	auto u = glGetUniformLocation(p.get(), un);
	if (u == -1)
		win::bug("No uniform " + std::string(un));

	return u;
}

GLRendererBackend::GLRendererBackend(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll)
	: resources(TextureDefinitions.textures.size())
	, text_renderer(screen_dims, projection, GL_TEXTURE2, true)
{
	resources.clear();
	const auto vertices = load_resources(roll);
	const auto &positions = std::get<0>(vertices);
	const auto &texcoords = std::get<1>(vertices);
	const auto &indices = std::get<2>(vertices);

	this->projection = glm::ortho(projection.left, projection.right, projection.bottom, projection.top);

	mode.atlas.program = std::move(win::GLProgram(win::load_gl_shaders(roll["shader/quad.vert"], roll["shader/quad.frag"])));
	glUseProgram(mode.atlas.program.get());
	mode.atlas.uniform_view_projection = get_uniform(mode.atlas.program, "view_projection");
	mode.atlas.uniform_position = get_uniform(mode.atlas.program, "position");
	mode.atlas.uniform_size = get_uniform(mode.atlas.program, "size");
	glUniformMatrix4fv(mode.atlas.uniform_view_projection, 1, GL_FALSE, glm::value_ptr(this->projection));

	mode.floor.program = std::move(win::GLProgram(win::load_gl_shaders(roll["shader/floor.vert"], roll["shader/floor.frag"])));
	glUseProgram(mode.floor.program.get());
	mode.floor.uniform_view_projection = get_uniform(mode.floor.program, "view_projection");
	mode.floor.uniform_position = get_uniform(mode.floor.program, "position");
	mode.floor.uniform_size = get_uniform(mode.floor.program, "size");
	mode.floor.uniform_layer = get_uniform(mode.floor.program, "layer");
	mode.floor.uniform_tc_scale = get_uniform(mode.floor.program, "tc_scale");
	glUniformMatrix4fv(mode.floor.uniform_view_projection, 1, GL_FALSE, glm::value_ptr(this->projection));
	glUniform1i(get_uniform(mode.floor.program, "tex"), 1);

	glBindVertexArray(vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords.get());
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint16_t) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices.get());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned char) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glClearColor(0.4f, 0.0f, 0.1f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	//const auto view = glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(-x, -y, 0.0f)), glm::vec3(zoom, zoom, zoom));
	const auto ident = glm::identity<glm::mat4>();
	const auto translate = glm::translate(ident, glm::vec3(-x, -y, 0.0f));
	const auto scale = glm::scale(ident, glm::vec3(zoom, zoom, 0.0f));
	const auto view = scale * translate;
	const auto vp = projection * view;

	glUseProgram(mode.floor.program.get());
	glUniformMatrix4fv(mode.floor.uniform_view_projection, 1, GL_FALSE, glm::value_ptr(vp));

	glUseProgram(mode.atlas.program.get());
	glUniformMatrix4fv(mode.atlas.uniform_view_projection, 1, GL_FALSE, glm::value_ptr(vp));
}

void GLRendererBackend::begin()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLRendererBackend::end()
{
}

void GLRendererBackend::render(const std::vector<Renderable> &objects)
{
	GLuint current_program = std::numeric_limits<GLuint>::max();

	glBindVertexArray(vao.get());

	int shader_switches = 0;

	for (const auto &obj : objects)
	{
		auto &resource = resources.at(obj.texture);

		if (resource.type == GLResourceType::floor)
		{
			if (current_program != mode.floor.program.get())
			{
				current_program = mode.floor.program.get();
				glUseProgram(current_program);
				++shader_switches;
			}

			glUniform2f(mode.floor.uniform_position, obj.x + (obj.w / 2.0f), obj.y + (obj.h / 2.0f));
			glUniform2f(mode.floor.uniform_size, obj.w, obj.h);
			glUniform1f(mode.floor.uniform_layer, resource.properties.floor_layer);
			glUniform2f(mode.floor.uniform_tc_scale, obj.w, obj.h);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*)(sizeof(std::uint8_t) * 6 * resource.vbo_offset));
		}
		else
		{
			// atlas

			if (current_program != mode.atlas.program.get())
			{
				current_program = mode.atlas.program.get();
				glUseProgram(current_program);
				++shader_switches;
			}

			glUniform2f(mode.atlas.uniform_position, obj.x + (obj.w / 2.0f), obj.y + (obj.h / 2.0f));
			glUniform2f(mode.atlas.uniform_size, obj.w, obj.h);

			glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*)(sizeof(std::uint8_t) * 6 * resource.vbo_offset), 0);
		}
	}

	if (shader_switches > 2)
		fprintf(stderr, "wow busy boy more than 2 shader switches\n");

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

std::tuple<std::vector<float>, std::vector<std::uint16_t>, std::vector<std::uint8_t>> GLRendererBackend::load_resources(win::AssetRoll &roll)
{
	if (TextureDefinitions.textures.size() > 42)
		win::bug("need to bump up gl index size");

	std::tuple<std::vector<float>, std::vector<std::uint16_t>, std::vector<std::uint8_t>> result(
		std::move(std::vector<float>(4 * 2 * TextureDefinitions.textures.size())),
		std::move(std::vector<std::uint16_t>(4 * 2 * TextureDefinitions.textures.size())),
		std::move(std::vector<std::uint8_t>(6 * TextureDefinitions.textures.size()))
	);

	auto &positions = std::get<0>(result);
	auto &texcoords = std::get<1>(result);
	auto &indices = std::get<2>(result);

	positions.clear();
	texcoords.clear();
	indices.clear();

	// load atlases
	if (TextureDefinitions.atlases.size() != 1) win::bug("only one atlas is supported atm");
	glActiveTexture(GL_TEXTURE0);
	for (const auto path : TextureDefinitions.atlases)
		atlases.add(roll[path], win::GLAtlas::Mode::linear);

	std::unique_ptr<std::uint8_t[]> floor_data;
	int floor_width = 0;
	int floor_height = 0;

	int floor_index = 0;
	int index = 0;
	for (const auto &def : TextureDefinitions.textures)
	{
		auto &resource = resources.emplace_back();

		if (def.atlas == -1)
		{
			win::Targa tga(roll[def.resource]);
			if (tga.bpp() != 32)
				win::bug("bad bpp");

			if (!floor_data)
			{
				floor_data = std::move(std::unique_ptr<std::uint8_t[]>(new uint8_t[tga.width() * tga.height() * 4 * TextureDefinitions.floor_textures.size()]));
				floor_width = tga.width();
				floor_height = tga.height();
			}

			if (tga.width() != floor_width || tga.height() != floor_height)
				win::bug("All floor textures must be the same size");

			resource.type = GLResourceType::floor;
			resource.vbo_offset = index;
			resource.properties.floor_layer = (float)index;

			memcpy(floor_data.get() + (floor_width * floor_height * 4 * floor_index), tga.data(), tga.width() * tga.height() * 4);

			generate_vertices(positions, texcoords, indices, 0.0f, 1.0f, 0.0f, 1.0f);

			++floor_index;
		}
		else
		{
			const auto &atlas = get_atlas(def.atlas);

			resource.type = GLResourceType::atlas;
			resource.vbo_offset = index;
			resource.properties.atlas = &atlas;

			const auto &item = atlas.item(def.atlas_index);
			generate_vertices(positions, texcoords, indices, item.x1, item.x2, item.y1, item.y2);
		}

		++index;
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mode.floor.tex.get());
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, floor_width, floor_height, TextureDefinitions.floor_textures.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, floor_data ? floor_data.get() : NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (texcoords.size() != 4 * 2 * TextureDefinitions.textures.size())
		win::bug("thbtbthbtbth");

	return result;
}

win::GLAtlas &GLRendererBackend::get_atlas(int index)
{
	int i = 0;
	for (auto &atlas : atlases)
	{
		if (i == index)
			return atlas;

		++i;
	}

	win::bug("no atlas for index " + std::to_string(index));
}

void GLRendererBackend::generate_vertices(std::vector<float> &pos, std::vector<std::uint16_t> &tc, std::vector<std::uint8_t> &indices, float left, float right, float bottom, float top)
{
	const auto max = std::numeric_limits<std::uint16_t>::max();

	pos.push_back(-0.5f); pos.push_back(0.5f);
	pos.push_back(-0.5f); pos.push_back(-0.5f);
	pos.push_back(0.5f); pos.push_back(-0.5f);
	pos.push_back(0.5f); pos.push_back(0.5f);

	tc.push_back((std::uint16_t)(left * max)); tc.push_back((std::uint16_t)(top * max));
	tc.push_back((std::uint16_t)(left * max)); tc.push_back((std::uint16_t)(bottom * max));
	tc.push_back((std::uint16_t)(right * max)); tc.push_back((std::uint16_t)(bottom * max));
	tc.push_back((std::uint16_t)(right * max)); tc.push_back((std::uint16_t)(top * max));

	const auto start = indices.empty() ? 0 : (indices[indices.size() - 1] + 1);

	indices.push_back(start + 0);
	indices.push_back(start + 1);
	indices.push_back(start + 2);
	indices.push_back(start + 0);
	indices.push_back(start + 2);
	indices.push_back(start + 3);
}
