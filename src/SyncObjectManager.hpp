#pragma once

#include <atomic>

#include "Darktimes.hpp"

enum class SyncObjectState
{
	empty,
	writer_locked,
	ready,
	reader_locked
};

// private -- don't use directly
template <typename T> struct SyncObject : T
{
	NO_COPY_MOVE(SyncObject);

	SyncObject()
		: state(SyncObjectState::empty)
	{}

	std::atomic<SyncObjectState> state;
};

// good for multi-reader / single-writer
template <typename T> class SyncObjectManager
{
	NO_COPY_MOVE(SyncObjectManager);

	static constexpr int slots = 2;
	static_assert(slots > 0, "slots must be greater than 0");

public:
	SyncObjectManager() = default;

	T *writer_acquire()
	{
		for (int i = 0; i < slots; ++i)
		{
			SyncObjectState expected = SyncObjectState::empty;
			if (objects[i].state.compare_exchange_strong(expected, SyncObjectState::writer_locked))
				return objects + i;
		}

		return NULL;
	}

	void writer_release(T *t)
	{
		auto object = (SyncObject<T>*)t;
		object->state.store(SyncObjectState::ready);

		for (int i = 0; i < slots; ++i)
		{
			if (objects + i == object)
				continue;

			SyncObjectState expected = SyncObjectState::ready;
			objects[i].state.compare_exchange_strong(expected, SyncObjectState::empty);
		}
	}

	T *reader_acquire()
	{
		for (int i = 0; i < slots; ++i)
		{
			SyncObjectState expected = SyncObjectState::ready;
			if (objects[i].state.compare_exchange_strong(expected, SyncObjectState::reader_locked))
				return objects + i;
		}

		return NULL;
	}

	void reader_release(T *t)
	{
		auto object = (SyncObject<T>*)t;
		object->state.store(SyncObjectState::empty);
	}

private:
	SyncObject<T> objects[slots];
};
