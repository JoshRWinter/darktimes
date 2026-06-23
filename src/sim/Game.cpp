#define _USE_MATH_DEFINES
#include <cmath>

#include "Game.hpp"
#include "levelgen/LevelGenerator.hpp"
#include "system/Systems.hpp"

Game::Game(const std::function<void(LevelData &&)> &level_generated)
    : level_generated(level_generated)
{
    generate_level();
}

Game::~Game()
{
    reset();
}

void Game::play(Renderables &renderables, const win::Pair<float> &mouse, const std::vector<KeyEvent> &buttons)
{
    process_inputs(mouse, buttons);
    player_system(world, controls);

    for (const auto &r : world.renderables)
    {
        const auto &phys = r.entity.get<PhysicalComponent>();
        renderables.renderables.emplace_back(r.texture, phys.x, phys.y, phys.w, phys.h, phys.rot);
    }

    const auto &player = world.players.begin()->entity.get<PhysicalComponent>();
    renderables.centerx = player.x;
    renderables.centery = player.y;

    const float orbitx = 7.0f;
    const float orbity = 10.5f;
    static float orbit = 0.0f;
    static float dist = 0.6f;
    orbit += 0.05f;
    renderables.light_renderables.emplace_back(orbitx + std::cosf(orbit) * dist, orbity + std::sinf(orbit) * dist, 0.2f, win::Color<float>(1.0f, 0.0f, 1.0f));

    renderables.light_renderables.emplace_back(player.x + player.w / 2.0f + std::cos(player.rot - 0.5f) * 0.25f,
                                               player.y + player.h / 2.0f + std::sin(player.rot - 0.5f) * 0.25f,
                                               1.0f,
                                               win::Color(0.8f, 0.8f, 0.3f),
                                               player.rot);
}

void Game::reset()
{
    if (world.players.size() > 0)
        PlayerEntity::destroy(world, world.players.begin()->entity);

    for (auto &gen : world.generics)
    {
        auto &ent = gen.entity;
        world.generics.remove(ent.remove<GenericComponent>());
        world.physicals.remove(ent.remove<PhysicalComponent>());
    }

    if (world.generics.size() != 0)
        win::bug("Generic components left over");

    if (world.physicals.size() != 0)
        win::bug("Physicals components left over");

    if (world.renderables.size() != 0)
        win::bug("Renderable components left over");
}

void Game::process_inputs(const win::Pair<float> &mouse, const std::vector<KeyEvent> &buttons)
{
    controls.x = mouse.x;
    controls.y = mouse.y;

    bool up = matinputs.up;
    bool down = matinputs.down;
    bool left = matinputs.left;
    bool right = matinputs.right;

    for (const auto b : buttons)
    {
        switch (b.button)
        {
            case win::Button::w:
                matinputs.up = b.press;
                up = up || b.press;
                break;
            case win::Button::s:
                matinputs.down = b.press;
                down = down || b.press;
                break;
            case win::Button::a:
                matinputs.left = b.press;
                left = left || b.press;
                break;
            case win::Button::d:
                matinputs.right = b.press;
                right = right || b.press;
                break;
            default:
                break;
        }
    }

    controls.up = up;
    controls.down = down;
    controls.left = left;
    controls.right = right;
}

void Game::generate_level()
{
    LevelGenerator generator;
    generator.generate(69);

    LevelData data;

    for (const auto &f : generator.level_floors)
    {
        data.renderables.emplace_back(f.texture, f.x, f.y, f.w, f.h, 0.0f);
    }

    /*
    for (const auto &w : generator.level_walls)
    {
        renderables.emplace_back(Texture::debug, w.x, w.y, w.w, w.h, 0.0f);
    }
    */

    for (const auto &p : generator.level_props)
    {
        const auto newprop = correct_prop_orientation(p);
        data.renderables.emplace_back(p.texture, newprop.x, newprop.y, newprop.w, newprop.h, get_prop_rotation(newprop.side));
    }

    for (const auto &l : generator.level_lights)
    {
        data.lights.emplace_back(l.x, l.y, l.power, l.color, l.angle);
    }

    for (const auto &wall : generator.level_walls)
    {
        const float embiggen = 0.03f;
        data.occluders.emplace_back(wall.x - embiggen, wall.y - embiggen, wall.x - embiggen, wall.y + wall.h + embiggen);
        data.occluders.emplace_back(wall.x + wall.w + embiggen, wall.y - embiggen, wall.x + wall.w + embiggen, wall.y + wall.h + embiggen);
        data.occluders.emplace_back(wall.x - embiggen, wall.y + wall.h + embiggen, wall.x + wall.w + embiggen, wall.y + wall.h + embiggen);
        data.occluders.emplace_back(wall.x - embiggen, wall.y - embiggen, wall.x + wall.w + embiggen, wall.y - embiggen);
    }

    {
        float left = 0.0f;
        float right = 0.0f;
        float bottom = 0.0f;
        float top = 0.0f;

        for (const auto &wall : generator.level_walls)
        {
            left = std::min(left, wall.x);
            right = std::max(right, wall.x + wall.w);
            bottom = std::min(bottom, wall.y);
            top = std::max(top, wall.y + wall.h);
        }

        for (const auto &prop : generator.level_props)
        {
            if (!prop.solid)
                continue;

            left = std::min(left, prop.x);
            right = std::max(right, prop.x + prop.w);
            bottom = std::min(bottom, prop.y);
            top = std::max(top, prop.y + prop.h);
        }

        world.index.level.reset(1.0f, left, right, bottom, top);

        for (auto &wall : generator.level_walls)
        {
            auto &ent = world.entities.add("wall");
            auto &phys = ent.add(world.physicals.add(ent, wall.x, wall.y, wall.w, wall.h, 0.0f));
            ent.add(world.generics.add(ent));

            world.index.level.add(win::SpatialIndexLocation(phys.x, phys.y, phys.w, phys.h), phys);
        }

        for (const auto &prop : generator.level_props)
        {
            if (!prop.solid)
                continue;

            const auto shrink = 0.6f;
            const auto w = prop.w * shrink;
            const auto h = prop.h * shrink;
            const float x = prop.x + ((prop.w - w) / 2.0f);
            const float y = prop.y + ((prop.h - h) / 2.0f);

            auto &ent = world.entities.add("prop");
            auto &phys = ent.add(world.physicals.add(ent, x, y, w, h, 0.0f));
            ent.add(world.generics.add(ent));

            world.index.level.add(win::SpatialIndexLocation(phys.x, phys.y, phys.w, phys.h), phys);
        }
    }

    data.lights.emplace_back(-1.6f, 1.6f, 4, win::Color<float>(0.4f, 0.1f, 0.1f));
    data.lights.emplace_back(4.6f, 3.6f, 6, win::Color<float>(0.1f, 0.9f, 0.2f));
    data.lights.emplace_back(0.0f, 7.6f, 6, win::Color<float>(0.1f, 0.1f, 0.9f));
    level_generated(std::move(data));
}

LevelProp Game::correct_prop_orientation(const LevelProp &prop)
{
    LevelProp copy = prop;

    switch (prop.side)
    {
        case LevelSide::left:
        case LevelSide::right:
            break;
        case LevelSide::bottom:
        case LevelSide::top:
            copy.w = prop.h;
            copy.h = prop.w;
            break;
    }

    const float center_x = prop.x + (prop.w / 2.0f);
    const float center_y = prop.y + (prop.h / 2.0f);

    const float new_x = center_x - (copy.w / 2.0f);
    const float new_y = center_y - (copy.h / 2.0f);

    copy.x = new_x;
    copy.y = new_y;

    return copy;
}

float Game::get_prop_rotation(const LevelSide side)
{
    switch (side)
    {
        case LevelSide::left:
            return M_PI;
        case LevelSide::right:
            return 0.0f;
        case LevelSide::bottom:
            return (M_PI * 3.0f) / 2.0f;
        case LevelSide::top:
            return M_PI / 2.0f;
    }

    win::bug("lolnope");
}
