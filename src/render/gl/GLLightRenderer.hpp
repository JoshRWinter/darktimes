#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/SpatialIndex.hpp>
#include <win/Utility.hpp>

#include "../../Renderable.hpp"
#include "GLRendererBase.hpp"

class GLLightRenderer : GLRendererBase
{
    struct Light
    {
        Light(int index, float x, float y, float power, float r, float g, float b)
            : index(index)
            , x(x)
            , y(y)
            , power(power)
            , r(r)
            , g(g)
            , b(b)
        {
        }

        int index;
        float x, y, power, r, g, b;
    };

    WIN_NO_COPY_MOVE(GLLightRenderer);

    static constexpr int shadowmap_size = 2000;
    static constexpr int max_dynamic_lights = 5;

public:
    explicit GLLightRenderer(win::AssetRoll &roll);

    void set_view_projection(const glm::mat4 &view_projection);
    void resize(const win::Dimensions<int> &res);
    void load(const std::vector<LightOccluder> &occluders, const std::vector<LightRenderable> &static_lights);
    void render(const std::vector<LightRenderable> &dynamic_lights, GLuint fbo);

private:
    glm::mat4 view_projection;
    win::Dimensions<int> res;

    std::vector<Light> lights;
    std::vector<LightOccluder> occluders;
    win::SpatialIndex<LightOccluder> occindex;

    struct
    {
        win::GLProgram program;
        GLint uniform_light_count;
        win::GLBuffer occluders;
        win::GLBuffer lights;
        win::GLBuffer shadowmap;
    } shadower;

    struct
    {
        win::GLFramebuffer fbo;
        win::GLTexture tex;

        win::GLProgram program;
        GLint uniform_light_count;
        GLint uniform_transform;
        win::GLBuffer lights;
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
