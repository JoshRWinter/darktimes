#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/Targa.hpp>

#include "GLConstants.hpp"
#include "GLRendererBackend.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll)
    : projection(glm::ortho(area.left, area.right, area.bottom, area.top))
    , floor_renderer(roll, texture_map)
    , atlas_renderer(roll, texture_map)
    , light_renderer(roll)
    , post_renderer(roll, GLConstants::intermediate_framebuffer_texture_unit)
{
    fprintf(stderr, "%s\n%s\n%s\n\n", (const char *)glGetString(GL_VENDOR), (const char *)glGetString(GL_RENDERER), (const char *)glGetString(GL_VERSION));

    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.get());

        glActiveTexture(GLConstants::intermediate_framebuffer_texture_unit);
        glBindTexture(GL_TEXTURE_2D, fbotex.get());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex.get(), 0);

        const GLenum bufs[] { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, bufs);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    GLRendererBackend::resize(area, res);

    check_error();
}

void GLRendererBackend::resize(const win::Area<float> &area, const win::Dimensions<int> &res)
{
    projection = glm::ortho(area.left, area.right, area.bottom, area.top);

    glViewport(0, 0, res.width, res.height);
    glActiveTexture(GLConstants::intermediate_framebuffer_texture_unit);
    glBindTexture(GL_TEXTURE_2D, fbotex.get());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, res.width, res.height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    light_renderer.resize(res);

    check_error();
}

void GLRendererBackend::set_view(float x, float y, float angle, float zoom)
{
    const auto ident = glm::identity<glm::mat4>();
    const auto translate = glm::translate(ident, glm::vec3(-x, -y, 0.0f));
    const auto scale = glm::scale(ident, glm::vec3(zoom, zoom, 1.0f));
    const auto rotate = glm::rotate(ident, -(angle - (float)M_PI / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    const auto translate2 = glm::translate(ident, glm::vec3(0.0f, -1.0f, 0.0f));
    const auto view = scale * translate2 * rotate * translate;
    const auto vp = projection * view;

    floor_renderer.set_view_projection(vp);
    atlas_renderer.set_view_projection(vp);
    light_renderer.set_view_projection(vp);
}

void GLRendererBackend::load_statics(const std::vector<Renderable> &static_renderables,
                                     const std::vector<LightOccluder> &occluders,
                                     const std::vector<LightRenderable> &static_lights)
{
    loaded_statics.clear();

    std::vector<Renderable> floor_renderables, atlas_renderables;

    for (const auto &s : static_renderables)
    {
        if (texture_map[s.texture].atlas_index == -1)
            floor_renderables.push_back(s);
        else
            atlas_renderables.push_back(s);
    }

    const auto floor_statics = floor_renderer.load(floor_renderables);
    const auto atlas_statics = atlas_renderer.load(atlas_renderables, texture_map);

    int floor_index = 0;
    int atlas_index = 0;
    for (const auto &s : static_renderables)
    {
        if (texture_map[s.texture].atlas_index == -1)
            loaded_statics.emplace_back(StaticObject::Type::floor, floor_statics.at(floor_index++));
        else
            loaded_statics.emplace_back(StaticObject::Type::atlas, atlas_statics.at(atlas_index++));
    }

    light_renderer.load(occluders, static_lights);
}

void GLRendererBackend::render(const std::vector<int> &static_renderables,
                               const std::vector<Renderable> &dynamic_renderables,
                               const std::vector<int> &static_lights,
                               const std::vector<LightRenderable> &dynamic_lights)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.get());

    if (!static_renderables.empty())
    {
        renderable_ids.clear();
        renderable_ids.reserve(static_renderables.size());

        auto type = loaded_statics[static_renderables[0]].type;

        const auto flush = [this, &type]()
        {
            if (type == StaticObject::Type::floor)
                floor_renderer.render(renderable_ids);
            else
                atlas_renderer.render(renderable_ids);

            renderable_ids.clear();
        };

        for (const auto i : static_renderables)
        {
            const auto &object = loaded_statics.at(i);

            if (type != object.type)
            {
                flush();
                type = object.type;
            }

            renderable_ids.push_back(object.base_vertex);
        }

        if (!renderable_ids.empty())
            flush();
    }

    if (!dynamic_renderables.empty())
        atlas_renderer.render(dynamic_renderables);

    flicker += 1.0f;
    if (!static_lights.empty() || !dynamic_lights.empty())
        light_renderer.render(static_lights, dynamic_lights, fbo.get(), flicker);

    post_renderer.render();

    check_error();
}

void GLRendererBackend::check_error()
{
#ifndef NDEBUG
    win::gl_check_error();
#endif
}
