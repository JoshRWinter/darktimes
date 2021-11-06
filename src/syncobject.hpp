#ifndef SYNC_OBJECT_HPP
#define SYNC_OBJECT_HPP

#include <atomic>
#include <mutex>
#include <vector>

#include "darktimes.hpp"

template <typename T> class LargeSyncObject
{
public:
	NOCOPYMOVE(LargeSyncObject);

	LargeSyncObject() : isdirty(false) {}

	T *prepare()
	{
		return new T;
	}

	void set(T *t)
	{
		std::lock_guard lock(mutex);
		isdirty = true;
		resource.reset(t);
	}

	void get(std::unique_ptr<T> &ret)
	{
		std::lock_guard lock(mutex);
		isdirty = false;
		ret.reset(resource.release());
	}

	bool dirty()
	{
		return isdirty;
	}

private:
	std::atomic<bool> isdirty;
	std::unique_ptr<T> resource;
	std::mutex mutex;
};

struct STR_LevelDataSyncObject
{
	std::vector<float> wall_verts;
	std::vector<float> floor_verts;
};

#endif
