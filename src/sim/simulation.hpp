#ifndef WORLD_SIMULATION_HPP
#define WORLD_SIMULATION_HPP

#include <atomic>
#include <mutex>
#include <memory>

#include "../syncobject.hpp"

void simulation(std::atomic<bool>&, LargeSyncObject<STR_LevelDataSyncObject>&);

#endif
