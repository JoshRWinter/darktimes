#pragma once

#include <atomic>
#include <mutex>
#include <memory>

#include "../syncobjectmanager.hpp"
#include "../levelrenderstate.hpp"
#include "../renderstate.hpp"
#include "../input.hpp"

void simulation(
	std::atomic<bool>&,
	SyncObjectManager<LevelRenderState>&,
	SyncObjectManager<RenderState>&,
	SyncObjectManager<Input>&
);
