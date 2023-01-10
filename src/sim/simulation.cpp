#include <thread>
#include <chrono>

#include "simulation.hpp"
#include "levelgen/levelmanager.hpp"

static void set_level_data_sync(SyncObjectManager<LevelDataSyncObject> &level_data_sync_object_manager, LevelManager &levelmanager)
{
	LevelDataSyncObject *ldso;
	do
	{
		ldso = level_data_sync_object_manager.writer_acquire();
	} while (ldso == NULL);

	ldso->walls = levelmanager.walls;
	ldso->floors = levelmanager.floors;
	ldso->props = levelmanager.props;
	ldso->seed = levelmanager.seed;

	level_data_sync_object_manager.writer_release(ldso);
}

void simulation(std::atomic<bool>& stop, SyncObjectManager<LevelDataSyncObject> &level_data_sync_object_manager)
{
	LevelManager level_manager(time(NULL));
	set_level_data_sync(level_data_sync_object_manager, level_manager);

	while(!stop)
	{
		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(10));

		level_manager.generate();
		set_level_data_sync(level_data_sync_object_manager, level_manager);
	}
}
