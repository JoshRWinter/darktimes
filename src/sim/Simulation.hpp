#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include <win/ConcurrentRingBuffer.hpp>
#include <win/SimStateExchanger.hpp>
#include <win/Win.hpp>

#include "../KeyEvent.hpp"
#include "../LevelData.hpp"
#include "../MouseInput.hpp"
#include "../Renderable.hpp"

class Simulation
{
    WIN_NO_COPY_MOVE(Simulation);

public:
    explicit Simulation(win::SimStateExchanger<Renderables> &simexchanger);
    ~Simulation();

    void queue_inputs(const std::vector<KeyEvent> &i);
    void set_mouse_input(const MouseInput &p);
    LevelData *get_leveldata();
    void release_leveldata(LevelData *renderables);

private:
    static void simulation(Simulation &sim);

    std::atomic<bool> stop_flag;
    win::ConcurrentRingBuffer<KeyEvent, 20> inputs;
    win::ObjectExchanger<MouseInput, 3> mouseinput;
    win::ObjectExchanger<LevelData, 1> leveldata;
    win::SimStateExchanger<Renderables> &simexchanger;
    std::thread thread;
};
