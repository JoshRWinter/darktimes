#pragma once

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "../../Renderable.hpp"
#include "GLRendererBase.hpp"

class GLLightRenderer : GLRendererBase
{
    WIN_NO_COPY_MOVE(GLLightRenderer);

    static constexpr int shadowmap_size = 2000;
    static constexpr int max_lights = 5;

public:
    explicit GLLightRenderer(win::AssetRoll &roll);

    void set_view_projection(const glm::mat4 &view_projection);
    void resize(const win::Dimensions<int> &res);
    void render(const std::vector<LightOccluder> &occluders, const std::vector<LightRenderable> &lights, GLuint fbo);

private:
    glm::mat4 view_projection;
    win::Dimensions<int> res;

    struct
    {
        win::GLProgram program;
        GLint uniform_occluder_count;
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
