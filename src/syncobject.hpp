#ifndef SYNC_OBJECT_HPP
#define SYNC_OBJECT_HPP

#include <atomic>
#include <mutex>

#include "darktimes.hpp"

template <typename T> class LargeSyncObject
{
public:
	NOCOPYMOVE(LargeSyncObject);

	LargeSyncObject() : isdirty(false) {}

	void set(const T &object)
	{
		std::lock_guard lock(mutex);
		dirty = true;
		resource.reset(new T);
		(*resource) = object;
	}

	void get(std::unique_ptr<T> &ret)
	{
		std::lock_guard lock(mutex);
		dirty = false;
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

class STR_LevelDataSyncObject
{
};

#endif
