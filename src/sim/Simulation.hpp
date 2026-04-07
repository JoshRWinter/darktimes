#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include <win/ConcurrentRingBuffer.hpp>
#include <win/SimStateExchanger.hpp>
#include <win/Win.hpp>

#include "../KeyEvent.hpp"
#include "../Renderable.hpp"

class Simulation
{
    WIN_NO_COPY_MOVE(Simulation);

public:
    explicit Simulation(win::SimStateExchanger<Renderables> &simexchanger);
    ~Simulation();

    void queue_inputs(const std::vector<KeyEvent> &i);
    void set_mouse_input(const win::Pair<float> &p);
    std::vector<Renderable> *get_statics();
    void release_statics(std::vector<Renderable> *renderables);

private:
    static void simulation(Simulation &sim);

    std::atomic<bool> stop_flag;
    win::ConcurrentRingBuffer<KeyEvent, 20> inputs;
    win::ObjectExchanger<win::Pair<float>, 3> mouseinput;
    win::ObjectExchanger<std::vector<Renderable>, 1> statics;
    win::SimStateExchanger<Renderables> &simexchanger;
    std::thread thread;
};
