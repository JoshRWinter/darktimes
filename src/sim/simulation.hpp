#ifndef WORLD_SIMULATION_HPP
#define WORLD_SIMULATION_HPP

#include <atomic>
#include <mutex>
#include <memory>

#include "../syncobjectmanager.hpp"
#include "../leveldatasyncobject.hpp"

void simulation(std::atomic<bool>&, SyncObjectManager<LevelDataSyncObject>&);

#endif
