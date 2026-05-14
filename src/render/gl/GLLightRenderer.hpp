#pragma once

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "../../Renderable.hpp"
#include "GLRendererBase.hpp"

class GLLightRenderer : GLRendererBase
{
    WIN_NO_COPY_MOVE(GLLightRenderer);

    static constexpr float shadowmap_size = 2000;

public:
    explicit GLLightRenderer(win::AssetRoll &roll);

    void set_view_projection(const glm::mat4 &view_projection);
    void resize(const win::Dimensions<int> &res);
    void render(const std::vector<LightOccluder> &occluders, float x, float y);

private:
    glm::mat4 view_projection;
    win::Dimensions<int> res;

    struct
    {
        win::GLProgram program;
        GLint uniform_occluder_count;
        GLint uniform_light;
        win::GLBuffer occluders;
        win::GLBuffer shadowmap;
    } shadower;

    struct
    {
        win::GLProgram program;
        GLint uniform_light;
        GLint uniform_transform;
        win::GLVertexArray vao;
    } lighter;
};
