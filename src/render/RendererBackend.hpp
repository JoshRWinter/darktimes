#pragma once

#include <vector>

#include "../Renderable.hpp"

class RendererBackend
{
public:
    virtual ~RendererBackend() = default;

    virtual void resize(const win::Area<float> &area, const win::Dimensions<int> &dims) = 0;
    virtual void set_view(float x, float y, float zoom) = 0;
    virtual void load_statics(const std::vector<Renderable> &statics,
                              const std::vector<LightOccluder> &occluders,
                              const std::vector<LightRenderable> &lights) = 0;
    virtual void render(const std::vector<int> &static_renderables,
                        const std::vector<Renderable> &dynamic_renderables,
                        const std::vector<int> &static_lights,
                        const std::vector<LightRenderable> &dynamic_lights) = 0;
};
