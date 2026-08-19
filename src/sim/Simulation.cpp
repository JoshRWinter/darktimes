#include <chrono>
#include <thread>

#include <win/Utility.hpp>

#include "../LevelData.hpp"
#include "Game.hpp"
#include "Simulation.hpp"

Simulation::Simulation(win::SimStateExchanger<Renderables> &simexchanger)
    : stop_flag(false)
    , simexchanger(simexchanger)
    , thread(simulation, std::ref(*this))
{
}

Simulation::~Simulation()
{
    stop_flag = true;
    thread.join();
}

void Simulation::queue_inputs(const std::vector<KeyEvent> &i)
{
    int put = 0;
    do
    {
        put += inputs.write(i.data() + put, i.size() - put);
    } while (put != i.size());
}

void Simulation::set_mouse_input(const MouseInput &mi)
{
    MouseInput *x;
    do
    {
        x = mouseinput.writer_acquire();
    } while (x == NULL);

    *x = mi;

    mouseinput.writer_release(x);
}

LevelData *Simulation::get_leveldata()
{
    return leveldata.reader_acquire();
}

void Simulation::release_leveldata(LevelData *renderables)
{
    leveldata.reader_release(renderables);
}

void Simulation::simulation(Simulation &sim)
{
    const std::function<void(LevelData && data)> level_generated = [&sim](LevelData &&data)
    {
        LevelData *leveldata;
        do
        {
            leveldata = sim.leveldata.writer_acquire();
        } while (leveldata == NULL);

        *leveldata = std::move(data);
        sim.leveldata.writer_release(leveldata);
    };

    Game game(level_generated);

    MouseInput mouse;
    std::vector<KeyEvent> buttons;
    buttons.reserve(decltype(sim.inputs)::length());

    while (!sim.stop_flag.load())
    {
        const auto i = sim.mouseinput.reader_acquire();
        if (i != NULL)
        {
            mouse = *i;
            sim.mouseinput.reader_release(i);
        }

        buttons.resize(decltype(sim.inputs)::length());
        const auto read = sim.inputs.read(buttons.data(), buttons.size());
        buttons.resize(read);

        auto &renderables = sim.simexchanger.prepare_simstate();
        renderables.clear();

        game.play(renderables, mouse, buttons);

        sim.simexchanger.release_simstate_and_sleep(renderables);
    }
}
