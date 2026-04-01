#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLConstants.hpp"
#include "GLFloorRenderer.hpp"

using namespace win::gl;

GLFloorRenderer::GLFloorRenderer(win::AssetRoll &roll, const TextureAssetMap &texture_map)
{
    init_textures(roll, texture_map);

    program = win::GLProgram(win::gl_load_shaders(roll["shader/static_floor.vert"], roll["shader/static_floor.frag"]));
    glUseProgram(program.get());
    uniform_view_projection = get_uniform(program, "view_projection");
    const auto uniform_tex = get_uniform(program, "tex");
    glUniform1i(uniform_tex, GLConstants::floor_texture_unit - GL_TEXTURE0);

    glBindVertexArray(vao.get());

    glBindBuffer(GL_ARRAY_BUFFER, position_texcoord.get());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(sizeof(float) * 2));

    glBindBuffer(GL_ARRAY_BUFFER, layer.get());
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, 0, NULL);

    check_error();
}

void GLFloorRenderer::set_view_projection(const glm::mat4 &view)
{
    glUseProgram(program.get());
    glUniformMatrix4fv(uniform_view_projection, 1, GL_FALSE, glm::value_ptr(view));
}

void GLFloorRenderer::render(const std::vector<std::uint16_t> &ids)
{
    glUseProgram(program.get());
    glBindVertexArray(vao.get());

    for (const auto base_vertex : ids)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, base_vertex, 4);
    }

    check_error();
}

std::vector<std::uint16_t> GLFloorRenderer::load(const std::vector<Renderable> &renderables)
{
    std::vector<std::uint16_t> results;

    // clang-format off
	static const float verts[] =
	{
		-0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f
	};
    // clang-format on

    static const auto ident = glm::identity<glm::mat4>();

    std::vector<float> position_texcoord_data;
    std::vector<std::uint8_t> layer_data;

    int count = 0;
    for (const auto &renderable : renderables)
    {
        const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 0.0f));
        const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f), renderable.y + (renderable.h / 2.0f), 0.0f));

        for (int i = 0; i < 4; ++i)
        {
            const glm::vec4 raw(verts[i * 4 + 0], verts[i * 4 + 1], 0.0f, 1.0f);
            const glm::vec2 tc(verts[i * 4 + 2], verts[i * 4 + 3]);
            const auto transformed = translate * scale * raw;

            position_texcoord_data.push_back(transformed.x);
            position_texcoord_data.push_back(transformed.y);
            position_texcoord_data.push_back((tc.s * renderable.w) + renderable.x);
            position_texcoord_data.push_back((tc.t * renderable.h) + renderable.y);
        }

        const int layer_index = floortex_map[(int)renderable.texture];
        if (layer_index > std::numeric_limits<std::uint8_t>::max() || layer_index < 0)
            win::bug("out of range");

        layer_data.push_back(layer_index);
        layer_data.push_back(layer_index);
        layer_data.push_back(layer_index);
        layer_data.push_back(layer_index);

        results.push_back(count * 4);
        ++count;
    }

    glBindVertexArray(vao.get());

    glBindBuffer(GL_ARRAY_BUFFER, position_texcoord.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_texcoord_data.size(), position_texcoord_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, layer.get());
    glBufferData(GL_ARRAY_BUFFER, sizeof(std::uint8_t) * layer_data.size(), layer_data.data(), GL_STATIC_DRAW);

    return results;
}

void GLFloorRenderer::init_textures(win::AssetRoll &roll, const TextureAssetMap &texture_map)
{
    glActiveTexture(GLConstants::floor_texture_unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, floortex.get());
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int count = 0;
    for (int i = 0; i < texture_map.size(); ++i)
    {
        if (texture_map[(Texture)i].atlas_index == -1)
            ++count;
    }

    int z = 0;
    int w = 0, h = 0;
    for (int i = 0; i < texture_map.size(); ++i)
    {
        const auto &item = texture_map[(Texture)i];
        if (item.atlas_index != -1)
        {
            floortex_map[i] = -1;
            continue;
        }

        win::Targa tga(roll[item.asset_path]);
        GLenum format;
        switch (tga.bpp())
        {
            case 8:
                format = GL_RED;
                break;
            case 24:
                format = GL_BGR;
                break;
            default:
                win::bug("Unsupported color depth: " + std::to_string(tga.bpp()));
        }

        if (z == 0)
        {
            w = tga.width();
            h = tga.height();
            glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB, w, h, count, 0, format, GL_UNSIGNED_BYTE, NULL);
        }

        if (tga.width() != w || tga.height() != h)
            win::bug("Floor textures must be " + std::to_string(w) + "x" + std::to_string(h));

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, z, w, h, 1, format, GL_UNSIGNED_BYTE, tga.data());
        floortex_map[i] = z;
        ++z;
    }
}
