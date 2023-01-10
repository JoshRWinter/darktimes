#pragma once

#include <atomic>
#include <mutex>
#include <memory>

#include "../syncobjectmanager.hpp"
#include "../leveldatasyncobject.hpp"

void simulation(std::atomic<bool>&, SyncObjectManager<LevelDataSyncObject>&);
