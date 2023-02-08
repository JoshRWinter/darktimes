#pragma once

#include "SyncObjectManager.hpp"

struct Input
{
	Input()
		: up(false)
		, down(false)
		, left(false)
		, right(false)
		, regenerate(false)
	{}

	bool up;
	bool down;
	bool left;
	bool right;
	bool regenerate;
};
