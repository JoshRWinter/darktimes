#pragma once

#include <atomic>
#include <mutex>
#include <memory>

#include "../syncobjectmanager.hpp"
#include "../leveldata.hpp"
#include "../input.hpp"
#include "../renderstate.hpp"

void simulation(std::atomic<bool>&, SyncObjectManager<LevelData>&, SyncObjectManager<Input>&, SyncObjectManager<RenderState>&);
