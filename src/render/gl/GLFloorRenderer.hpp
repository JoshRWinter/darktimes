#pragma once

#include <vector>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Renderable.hpp"
#include "../TextureAssetMap.hpp"
#include "GLRendererBase.hpp"

class GLFloorRenderer : GLRendererBase
{
public:
    explicit GLFloorRenderer(win::AssetRoll &roll, const TextureAssetMap &texture_map);

    void set_view_projection(const glm::mat4 &view);
    void render(const std::vector<std::uint16_t> &ids);
    std::vector<std::uint16_t> load(const std::vector<Renderable> &renderables);

private:
    void init_textures(win::AssetRoll &roll, const TextureAssetMap &texture_map);

    win::GLTexture floortex;
    int floortex_map[TextureAssetMap::size()];

    win::GLProgram program;
    int uniform_view_projection;

    win::GLVertexArray vao;
    win::GLBuffer position_texcoord, layer;
};
