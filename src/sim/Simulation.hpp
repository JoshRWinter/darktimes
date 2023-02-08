#pragma once

#include <atomic>
#include <mutex>
#include <memory>

#include "../SyncObjectManager.hpp"
#include "../LevelRenderState.hpp"
#include "../RenderState.hpp"
#include "../Input.hpp"

void simulation(
	std::atomic<bool>&,
	SyncObjectManager<LevelRenderState>&,
	SyncObjectManager<RenderState>&,
	SyncObjectManager<Input>&
);
