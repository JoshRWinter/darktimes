#include <chrono>
#include <thread>

#include <win/Utility.hpp>

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

void Simulation::queue_inputs(const std::vector<win::Button> &i)
{
	int put = 0;
	do
	{
		put += inputs.write(i.data() + put, i.size() - put);
	} while (put != i.size());
}

void Simulation::set_mouse_input(const win::Pair<float> &p)
{
	win::Pair<float> *x;
	do
	{
		x = mouseinput.writer_acquire();
	} while (x == NULL);

	*x = p;

	mouseinput.writer_release(x);
}

std::vector<Renderable> *Simulation::get_statics()
{
	return statics.reader_acquire();
}

void Simulation::release_statics(std::vector<Renderable> *renderables)
{
	statics.reader_release(renderables);
}

void Simulation::simulation(Simulation &sim)
{
	const std::function<void(const std::vector<Renderable>&)> level_generated = [&sim](const std::vector<Renderable> &r)
	{
		std::vector<Renderable> *renderables;
		do
		{
			renderables = sim.statics.writer_acquire();
		} while (renderables == NULL);

		*renderables = r;
		sim.statics.writer_release(renderables);
	};

	Game game(level_generated);
	game.reset();

	win::Pair<float> mouse;
	std::vector<win::Button> buttons;
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
		buttons.clear();
		const auto read = sim.inputs.read(buttons.data(), buttons.size());
		buttons.resize(read);

		auto &renderables = sim.simexchanger.prepare_simstate();
		renderables.clear();

 		game.play(renderables, mouse, buttons);

		sim.simexchanger.release_simstate_and_sleep(renderables);
	}
}
