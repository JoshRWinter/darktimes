#include "WorldSimulation.hpp"
#include <unistd.h>

void simulation(std::atomic<bool>& stop, SyncObject<SimulationToRendererLevelDataSyncObject> &syncobject_str_level_data)
{
	while(!stop)
	{
		usleep(100000);
	}
}
