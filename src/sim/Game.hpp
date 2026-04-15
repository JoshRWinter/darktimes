#pragma once

#include <functional>
#include <vector>

#include <win/Utility.hpp>
#include <win/Win.hpp>

#include "../KeyEvent.hpp"
#include "../Renderable.hpp"
#include "Controls.hpp"
#include "levelgen/LevelObjects.hpp"
#include "World.hpp"

struct MaterializedInputs
{
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool fire = false;
};

class Game
{
    WIN_NO_COPY_MOVE(Game);

public:
    explicit Game(const std::function<void(const std::vector<Renderable> &)> &level_generated);
    ~Game();

    void play(Renderables &renderables, const win::Pair<float> &mouse, const std::vector<KeyEvent> &buttons);
    void reset();

private:
    void process_inputs(const win::Pair<float> &mouse, const std::vector<KeyEvent> &buttons);
    void generate_level();
    static LevelProp correct_prop_orientation(const LevelProp &prop);
    static float get_prop_rotation(LevelSide side);

    MaterializedInputs matinputs;
    Controls controls;
    World world;

    const std::function<void(const std::vector<Renderable> &)> &level_generated;
};
