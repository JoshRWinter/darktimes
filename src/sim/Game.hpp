#pragma once

#include <functional>
#include <vector>

#include <win/Utility.hpp>
#include <win/Win.hpp>

#include "../Renderable.hpp"
#include "levelgen/LevelObjects.hpp"
#include "World.hpp"

class Game
{
    WIN_NO_COPY_MOVE(Game);

public:
    explicit Game(const std::function<void(const std::vector<Renderable> &)> &level_generated);

    void play(Renderables &renderables, const win::Pair<float> &mouse, const std::vector<win::Button> &buttons);
    void reset();

private:
    void generate_level();
    static LevelProp correct_prop_orientation(const LevelProp &prop);
    static float get_prop_rotation(LevelSide side);

    World world;
    const std::function<void(const std::vector<Renderable> &)> &level_generated;
};
