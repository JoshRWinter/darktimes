#include <thread>
#include <chrono>

#include "Simulation.hpp"

#include "World.hpp"

#include "system/PlayerSystem.hpp"

Simulation::Simulation()
	: stop_flag(false)
	, thread(simulation, std::ref(*this))
{
}

Simulation::~Simulation()
{
	stop_flag = true;
	if (thread.joinable())
		thread.join();
}

void Simulation::reset(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, const std::vector<LevelProp> &props)
{
	SimulationResetCommand *so;
	while ((so = som_level_package.writer_acquire()) == NULL);

	so->floors = floors;
	so->walls = walls;
	so->props = props;

	som_level_package.writer_release(so);
}

void Simulation::set_input(const GameInput &input)
{
	GameInput *so;
	while ((so = som_input.writer_acquire()) == NULL);

	*so = input;
	som_input.writer_release(so);
}

RenderableWorldState *Simulation::get_state(RenderableWorldState *previous)
{
	if (previous != NULL)
		som_state.reader_release(previous);

	return som_state.reader_acquire();
}

void Simulation::simulation(Simulation &sim)
{
	World world;
	GameInput input;

	auto tick_start = std::chrono::high_resolution_clock::now();
	while (!sim.stop_flag)
	{
		// take int account newest input
		{
			GameInput *so;
			if ((so = sim.som_input.reader_acquire()) != NULL)
			{
				input = *so;
				sim.som_input.reader_release(so);
			}
		}

		// run the world simulation
		const auto state = world.tick(input);

		// upload the results;
		{
			RenderableWorldState *so;

			while ((so = sim.som_state.writer_acquire()) == NULL);
			*so = state;
			sim.som_state.writer_release(so);
		}

		// wait until time for next tick;
		while (std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - tick_start).count() < 16.66666f)
			std::this_thread::sleep_for(std::chrono::microseconds (1));

		tick_start = std::chrono::high_resolution_clock::now();
	}
}
