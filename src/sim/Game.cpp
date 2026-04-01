#include "Game.hpp"
#include "levelgen/LevelGenerator.hpp"

Game::Game(const std::function<void(const std::vector<Renderable> &)> &level_generated)
    : level_generated(level_generated)
{
}

void Game::play(Renderables &renderables, const win::Pair<float> &mouse, const std::vector<win::Button> &buttons) {}

void Game::reset()
{
    generate_level();
}

void Game::generate_level()
{
    LevelGenerator generator;
    generator.generate(69);

    std::vector<Renderable> renderables;
    for (const auto &f : generator.level_floors)
    {
        renderables.emplace_back(f.texture, f.x, f.y, f.w, f.h, 0.0f);
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
        renderables.emplace_back(p.texture, newprop.x, newprop.y, newprop.w, newprop.h, get_prop_rotation(newprop.side));
    }

    /*
    std::vector<win::Box<float>> occluders;
    for (const auto &wall : generator.level_walls)
        occluders.emplace_back(wall.x, wall.y, wall.w, wall.h);
        */

    level_generated(renderables);
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
