#include <thread>
#include <chrono>

#include "simulation.hpp"
#include "levelmanager.hpp"

static void set_level_data_sync(LargeSyncObject<STR_LevelDataSyncObject> &so, std::vector<float> &&verts)
{
	auto data = so.prepare();
	data->wall_verts = std::move(verts);
	so.set(data);
}

void simulation(std::atomic<bool>& stop, LargeSyncObject<STR_LevelDataSyncObject> &str_level_data_sync)
{
	LevelManager level_manager;
	set_level_data_sync(str_level_data_sync, std::move(level_manager.get_wall_verts()));

	while(!stop)
	{
		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(200));
	}
}
