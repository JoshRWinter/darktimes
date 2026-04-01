#pragma once

#include <vector>

#include "../Renderable.hpp"

class RendererBackend
{
public:
    virtual ~RendererBackend() = default;

    virtual void set_view(float x, float y, float zoom) = 0;
    virtual void load_statics(const std::vector<Renderable> &statics) = 0;
    virtual void render_statics(const std::vector<int> &statics) = 0;
    virtual void render_dynamics(const std::vector<Renderable> &dynamics) = 0;
};
