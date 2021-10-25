#ifndef WORLD_SIMULATION_HPP
#define WORLD_SIMULATION_HPP

#include <atomic>
#include <mutex>
#include <memory>

#include "Darktimes.hpp"

template <typename T> class SyncObject
{
public:
	NOCOPYMOVE(SyncObject);

	SyncObject() : dirty(false) {}

	void set(const T &object)
	{
		std::lock_guard lock(mutex);

		resource.reset(new T);
		(*resource) = object;
		dirty = true;
	}

	void get(std::unique_ptr<T> &ret)
	{
		std::lock_guard lock(mutex);

		if (resource && dirty)
		{
			ret.reset(resource.release);
			dirty = false;
		}
		else
			ret.reset();
	}

	bool dirty;
	std::unique_ptr<T> resource;
	std::mutex mutex;
};

class SimulationToRendererLevelDataSyncObject
{
public:
	NOCOPYMOVE(SimulationToRendererLevelDataSyncObject);
};

void simulation(std::atomic<bool>&, SyncObject<SimulationToRendererLevelDataSyncObject>&);

#endif
