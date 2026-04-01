#pragma once

#include <memory>
#include <vector>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "../Renderable.hpp"
#include "RendererBackend.hpp"

class Renderer
{
public:
    explicit Renderer(const win::Dimensions<int> &screen_dims, const win::Area<float> &projection, win::AssetRoll &roll);

    void set_statics(const std::vector<Renderable> &statics);
    void render(Renderables &renderables);

private:
    std::vector<Renderable> statics;
    std::unique_ptr<RendererBackend> backend;
};
