#define _USE_MATH_DEFINES
#include <cmath>

#include "gl/GLRendererBackend.hpp"
#include "Renderer.hpp"

Renderer::Renderer(const win::Area<float> &area, const win::Dimensions<int> &res, win::AssetRoll &roll)
    : static_renderables_staging(100)
    , static_lights_staging(10)
    , backend(new GLRendererBackend(area, res, roll))
{
}

void Renderer::set_leveldata(const LevelData &leveldata)
{
    int index = 0;
    for (const auto &r : leveldata.renderables)
        static_renderables.emplace_back(r, index++);

    static_lights = leveldata.lights;

    backend->load_statics(leveldata.renderables, leveldata.occluders, leveldata.lights);
}

void Renderer::render(const Renderables &prev, const Renderables &current, float lerp, float angle)
{
    const auto interpolate = [lerp](float a, float b)
    {
        return a + (b - a) * lerp;
    };

    const auto angle_interpolate = [lerp](float a, float b)
    {
        if (a > M_PI / 2.0f && b < -M_PI / 2.0f)
            b += M_PI * 2.0f;

        if (b > M_PI / 2.0f && a < -M_PI / 2.0f)
            a += M_PI * 2.0f;

        return a + (b - a) * lerp;
    };

    const float centerx = interpolate(prev.centerx, current.centerx);
    const float centery = interpolate(prev.centery, current.centery);

    backend->set_view(centerx, centery, -angle, 1.0f);

    {
        const auto nearby = [&](const auto &item, float radius_x, float radius_y)
        {
            return centerx + radius_x > item.x && centerx - radius_x < item.x + item.w && centery + radius_y > item.y && centery - radius_y < item.y + item.h;
        };

        static_renderables_staging.clear();
        for (const auto &r : static_renderables)
        {
            if (nearby(r, 12.0f, 8.0f))
                static_renderables_staging.push_back(r.index);
        }
    }

    {
        const auto nearby = [&](const auto &item, float dist)
        {
            return std::max(std::abs(item.x - prev.centerx), std::abs(item.y - prev.centery)) < dist;
        };

        static_lights_staging.clear();
        for (int i = 0; i < static_lights.size(); ++i)
        {
            if (nearby(static_lights[i], 15.0f))
                static_lights_staging.push_back(i);
        }
    }

    {
        dynamic_renderables_staging.clear();
        for (const auto &r : current.renderables)
        {
            const Renderable *p = NULL;
            if (r.id != -1)
            {
                for (const auto &x : prev.renderables)
                {
                    if (x.id == r.id)
                    {
                        p = &x;
                        break;
                    }
                }
            }

            if (p != NULL)
                dynamic_renderables_staging.emplace_back(r.id,
                                                         r.texture,
                                                         interpolate(p->x, r.x),
                                                         interpolate(p->y, r.y),
                                                         r.w,
                                                         r.h,
                                                         angle_interpolate(p->rot, r.rot));
            else
                dynamic_renderables_staging.emplace_back(r.id, r.texture, r.x, r.y, r.w, r.h, r.rot);
        }
    }

    {
        dynamic_lights_staging.clear();
        for (const auto &r : current.light_renderables)
        {
            const LightRenderable *p = NULL;
            if (r.id != -1)
            {
                for (const auto &x : prev.light_renderables)
                {
                    if (x.id == r.id)
                    {
                        p = &x;
                        break;
                    }
                }
            }

            if (p != NULL)
                dynamic_lights_staging.emplace_back(r.id,
                                                    interpolate(p->x, r.x),
                                                    interpolate(p->y, r.y),
                                                    interpolate(p->power, r.power),
                                                    r.color,
                                                    angle_interpolate(p->angle, r.angle),
                                                    r.primary);
            else
                dynamic_lights_staging.emplace_back(r.id, r.x, r.y, r.power, r.color, r.angle, r.primary);
        }
    }

    backend->render(static_renderables_staging, dynamic_renderables_staging, static_lights_staging, dynamic_lights_staging);
}

void Renderer::resize(const win::Area<float> &area, const win::Dimensions<int> &res)
{
    backend->resize(area, res);
}
