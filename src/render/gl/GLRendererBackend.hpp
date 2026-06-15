#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../RendererBackend.hpp"
#include "../TextureAssetMap.hpp"
#include "GLAtlasRenderer.hpp"
#include "GLFloorRenderer.hpp"
#include "GLLightRenderer.hpp"
#include "GLPostRenderer.hpp"

struct StaticObject
{
    enum class Type
    {
        floor,
        atlas
    };

    StaticObject(Type type, std::uint16_t base_vertex)
        : type(type)
        , base_vertex(base_vertex)
    {
    }

    Type type;
    std::uint16_t base_vertex;
};

class GLRendererBackend : public RendererBackend
{
    WIN_NO_COPY_MOVE(GLRendererBackend);

public:
    GLRendererBackend(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll);
    ~GLRendererBackend() override = default;

    void resize(const win::Area<float> &area, const win::Dimensions<int> &res) override;
    void set_view(float x, float y, float zoom) override;
    void load_statics(const std::vector<Renderable> &static_renderables,
                      const std::vector<LightOccluder> &occluders,
                      const std::vector<LightRenderable> &static_lights) override;
    void render(const std::vector<int> &static_renderables,
                const std::vector<Renderable> &dynamic_renderables,
                const std::vector<int> &static_lights,
                const std::vector<LightRenderable> &dynamic_lights) override;

private:
    static void check_error();

    glm::mat4 projection;

    TextureAssetMap texture_map;

    win::GLFramebuffer fbo;
    win::GLTexture fbotex;

    GLFloorRenderer floor_renderer;
    GLAtlasRenderer atlas_renderer;
    GLLightRenderer light_renderer;
    GLPostRenderer post_renderer;

    std::vector<StaticObject> loaded_statics;

    std::vector<std::uint16_t> renderable_ids;
};
