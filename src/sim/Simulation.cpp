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
	while ((so = som_level_package.writer_acquire()) == NULL)
	{
		so->floors = floors;
		so->walls = walls;
		so->props = props;

		som_level_package.writer_release(so);
	}
}

void Simulation::simulation(Simulation &sim)
{
	auto tick_start = std::chrono::high_resolution_clock::now();
	while (!sim.stop_flag)
	{
		sim.tick();

		while (std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - tick_start).count() < 16.66666f)
			std::this_thread::sleep_for(std::chrono::microseconds (1));

		tick_start = std::chrono::high_resolution_clock::now();
	}
}

void Simulation::tick()
{

}
