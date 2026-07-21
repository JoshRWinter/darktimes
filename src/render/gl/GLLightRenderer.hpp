#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GLMappedRingBuffer.hpp>
#include <win/SpatialIndex.hpp>
#include <win/Utility.hpp>

#include "../../Renderable.hpp"
#include "GLRendererBase.hpp"

class GLLightRenderer : GLRendererBase
{
    struct StaticLight
    {
        StaticLight(int index, float x, float y, float power, float r, float g, float b, float angle)
            : index(index)
            , x(x)
            , y(y)
            , power(power)
            , r(r)
            , g(g)
            , b(b)
            , angle(angle)
        {
        }

        int index;
        float x, y, power, r, g, b, angle;
    };

    struct ShadowerLight
    {
        int index;
        float x;
        float y;
        int occluder_start;
        int occluder_count;
    };

    struct LighterLight
    {
        int index;
        float x;
        float y;
        float power;
        float r;
        float g;
        float b;
        float angle;
    };

    WIN_NO_COPY_MOVE(GLLightRenderer);

    static constexpr int shadowmap_size = 2000;
    static constexpr int max_dynamic_lights = 5;
    static constexpr int lighter_light_buf_count = 50;
    static constexpr int shadower_light_buf_count = 50;
    static constexpr int occluder_buf_count = 4000;

public:
    explicit GLLightRenderer(win::AssetRoll &roll);

    void set_view_projection(const glm::mat4 &view_projection);
    void resize(const win::Dimensions<int> &res);
    void load(const std::vector<LightOccluder> &occluders, const std::vector<LightRenderable> &static_lights);
    void render(const std::vector<int> &static_lights, const std::vector<LightRenderable> &dynamic_lights, GLuint fbo, float flicker);

private:
    glm::mat4 view_projection;
    win::Dimensions<int> res;

    std::vector<StaticLight> lights;
    std::vector<LightOccluder> occluders;
    win::SpatialIndex<LightOccluder> occindex;

    std::array<float, 2000> flicker_pattern;

    struct
    {
        win::GLProgram program;
        GLint uniform_light_start;
        GLint uniform_light_count;
        GLint uniform_light_buffer_size;
        win::GLBuffer occluders;
        win::GLBuffer lights;
        win::GLMappedRingBuffer<ShadowerLight> lightbuf;
        win::GLMappedRingBuffer<LightOccluder> occbuf;
        std::vector<LightOccluder> occ_staging;
        std::vector<int> occ_ranges;
        win::GLBuffer shadowmap;
    } shadower;

    struct
    {
        win::GLFramebuffer fbo;
        win::GLTexture tex;

        win::GLProgram program;
        GLint uniform_primary_index;
        GLint uniform_light_start;
        GLint uniform_light_count;
        GLint uniform_transform;
        win::GLBuffer lights;
        win::GLMappedRingBuffer<LighterLight> lightbuf;
        win::GLVertexArray vao;
    } lighter;

    struct
    {
        win::GLFramebuffer fbo;
        win::GLTexture tex;

        win::GLProgram program;
        GLint uniform_tex;
        GLint uniform_horizontal;
        win::GLVertexArray vao;
    } blur;
};
