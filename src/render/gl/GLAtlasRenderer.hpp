#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/Pool.hpp>

#include "../../Renderable.hpp"
#include "../TextureAssetMap.hpp"
#include "GLRendererBase.hpp"

class GLAtlasRenderer : GLRendererBase
{
public:
    explicit GLAtlasRenderer(win::AssetRoll &roll, const TextureAssetMap &texture_map);

    void set_view_projection(const glm::mat4 &view_projection);
    void render(const std::vector<Renderable> &renderables);
    void render(const std::vector<std::uint16_t> &ids);
    std::vector<std::uint16_t> load(const std::vector<Renderable> &renderables, const TextureAssetMap &texture_map);

private:
    void init_atlases(win::AssetRoll &roll, const TextureAssetMap &map);
    glm::mat4 view_projection;

    std::vector<win::GLAtlas *> atlas_map;
    win::Pool<win::GLAtlas, 1, true> atlases;

    struct
    {
        win::GLProgram program;
        int uniform_view_projection;
        win::GLVertexArray vao;
        win::GLBuffer position, texcoord;
    } staticmode;

    struct
    {
        win::GLProgram program;
        int uniform_mvp;
        win::GLVertexArray vao;
        win::GLBuffer buffer;

        std::vector<int> base_vertex_map;
    } dynamicmode;
};
