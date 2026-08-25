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

void Game::play(Renderables &renderables, const MouseInput &mouse, const std::vector<KeyEvent> &buttons)
{
    process_inputs(mouse, buttons);
    player_system(world, controls);

    for (const auto &r : world.renderables)
    {
        const auto &phys = r.entity.get<PhysicalComponent>();
        renderables.renderables.emplace_back(r.id, r.texture, phys.x, phys.y, phys.w, phys.h, phys.rot);
    }

    for (const auto &r : world.light_renderables)
    {
        renderables.light_renderables.emplace_back(r.id, r.x, r.y, r.power, r.color, r.angle, r.primary);
    }

    const auto &player = world.players.begin()->entity.get<PhysicalComponent>();
    renderables.centerx = player.x + player.w / 2.0f;
    renderables.centery = player.y + player.h / 2.0f;
    renderables.angle = player.rot;
}

void Game::reset()
{
    world.reset();
}

void Game::process_inputs(const MouseInput &mouse, const std::vector<KeyEvent> &buttons)
{
    controls.x = mouse.x;
    controls.y = mouse.y;
    controls.pan = mouse.pan;

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
        data.renderables.emplace_back(-1, f.texture, f.x, f.y, f.w, f.h, 0.0f);
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
        data.renderables.emplace_back(-1, p.texture, newprop.x, newprop.y, newprop.w, newprop.h, get_prop_rotation(newprop.side));
        if (p.lightpower > 0.0f)
            data.lights.emplace_back(-1, p.lightx, p.lighty, p.lightpower, p.lightcolor, -1.0f, false);
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

            world.index.level.add(win::SpatialIndexLocation(phys.x, phys.y, phys.w, phys.h), phys);
        }
    }

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
