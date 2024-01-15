#pragma once

#include <thread>
#include <atomic>

#include "../Darktimes.hpp"
#include "../SyncObjectManager.hpp"
#include "../levelgen/LevelObjects.hpp"

struct SimulationResetCommand
{
	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;
	std::vector<LevelProp> props;
};

class Simulation
{
	NO_COPY_MOVE(Simulation);

public:
	// all public methods are called by instantiating thread
	Simulation();
	~Simulation();

	void reset(const std::vector<LevelFloor> &floors, const std::vector<LevelWall> &walls, const std::vector<LevelProp> &props);

private:
	// all private methods called by "thread" thread
	static void simulation(Simulation &sim);
	void tick();

	SyncObjectManager<SimulationResetCommand, 1> som_level_package;

	std::atomic<bool> stop_flag;
	std::thread thread;
};
