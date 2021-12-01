#include <thread>
#include <chrono>

#include "simulation.hpp"
#include "levelmanager.hpp"

static void set_level_data_sync(LargeSyncObject<STR_LevelDataSyncObject> &so, LevelManager &levelmanager)
{
	auto data = so.prepare();

	data->walls = levelmanager.walls;
	data->floors = levelmanager.floors;

	so.set(data);
}

void simulation(std::atomic<bool>& stop, LargeSyncObject<STR_LevelDataSyncObject> &str_level_data_sync)
{
	LevelManager level_manager(time(NULL));
	set_level_data_sync(str_level_data_sync, level_manager);

	while(!stop)
	{
		//std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1800));
		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(10));

		/*
		level_manager.reset();
		set_level_data_sync(str_level_data_sync, level_manager);
		*/
	}
}
