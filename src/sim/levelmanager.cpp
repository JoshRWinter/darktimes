#include <cmath>
#include <stack>
#include <algorithm>

#include "../darktimes.hpp"
#include "../assetmanager.hpp"
#include "levelmanager.hpp"

static bool float_equals(float a, float b, float tolerance = 0.005f)
{
	return std::fabs(a - b) < tolerance;
}

static std::vector<LevelFloorConnector> get_connectors(const std::vector<LevelFloorConnector> &connectors, LevelSide side)
{
	std::vector<LevelFloorConnector> found;

	for (const auto &connector : connectors)
		if (connector.side == side)
			found.push_back(connector);

	std::sort(found.begin(), found.end(), [](const LevelFloorConnector &a, const LevelFloorConnector &b)
	{
		return a.start < b.start;
	});

	return found;
}

static std::array<LevelSide, 4> enumerate_connector_sides()
{
	std::array<LevelSide, 4> sides;

	sides.at(0) = LevelSide::LEFT;
	sides.at(1) = LevelSide::RIGHT;
	sides.at(2) = LevelSide::BOTTOM;
	sides.at(3) = LevelSide::TOP;

	return sides;
}

static LevelSide flip(LevelSide side)
{
	switch (side)
	{
	case LevelSide::LEFT:
		return LevelSide::RIGHT;
	case LevelSide::RIGHT:
		return LevelSide::LEFT;
	case LevelSide::BOTTOM:
		return LevelSide::TOP;
	case LevelSide::TOP:
		return LevelSide::BOTTOM;
	}

	win::bug("no side");
}

LevelManager::LevelManager(int seed)
	: mersenne(seed)
	, seed(seed)
{
	fprintf(stderr, "%d\n", seed);
	generate();
}

void LevelManager::generate()
{
	reset();

	while (!generate_impl()) reset();

	generate_walls();
	generate_props();
}

bool LevelManager::generate_impl()
{
	floors.emplace_back(random_int(0, 1), -1.0f, -1.0f, 2.0f, 2.0f);

	bool first = true;
	while (floors.size() < 50)
	{
		std::vector<LevelFloor> generated;
		int attempts = 0;
		int start_candidate_index;
		LevelSide side = first ? LevelSide::TOP : random_side();
		do
		{
			start_candidate_index = find_start_candidate(floors, attempts++);

			if (start_candidate_index == -1)
				break;

			generated = prune(generate_grid(floors.at(start_candidate_index), side));
		} while (generated.size() < 5);

		if (start_candidate_index == -1) // failed to generate
			return false;

		connect(floors.at(start_candidate_index), *find_neighbors(generated, floors.at(start_candidate_index), side).at(0));

		for (const auto &floor : generated)
			floors.push_back(floor);

		first = false;

		attempts = 0;
		do
		{
			side = first ? LevelSide::TOP : random_side();
			start_candidate_index = find_start_candidate(floors, attempts++);

			if (start_candidate_index == -1)
				break;

			generated = generate_linear(floors.at(start_candidate_index), side);
		} while (generated.size() < 2);

		if (start_candidate_index == -1)
			return false;

		connect(floors.at(start_candidate_index), *find_neighbors(generated, floors.at(start_candidate_index), side).at(0));

		for (const auto &floor : generated)
			floors.push_back(floor);
	}

	return true;
}

int LevelManager::find_start_candidate(const std::vector<LevelFloor> &floors, const int attempt_num)
{
	if (attempt_num < 0)
		win::bug("invalid param");

	if (floors.empty())
		win::bug("no possible start candidate");

	if (floors.size() == 1)
		return 0;

	const int low = std::max(1, ((int)floors.size()) - 8);
	const int high = floors.size() - 1;

	std::vector<int> one_conn;
	std::vector<int> two_conn;
	std::vector<int> three_conn;

	for (int i = low; i <= high; ++i)
	{
		if (floors.at(i).connectors.size() == 1)
			one_conn.push_back(i);
		else if (floors.at(i).connectors.size() == 2)
			two_conn.push_back(i);
		else if (floors.at(i).connectors.size() == 3)
			three_conn.push_back(i);
	}

	std::shuffle(one_conn.begin(), one_conn.end(), mersenne);
	std::shuffle(two_conn.begin(), two_conn.end(), mersenne);
	std::shuffle(three_conn.begin(), three_conn.end(), mersenne);

	std::vector<int> candidates;

	for (int c : one_conn)
		candidates.push_back(c);
	for (int c : two_conn)
		candidates.push_back(c);
	for (int c : three_conn)
		candidates.push_back(c);

	if (attempt_num >= candidates.size())
		return -1;

	return candidates.at(attempt_num);
}

void LevelManager::reset()
{
	floors.clear();
	walls.clear();
}

std::vector<LevelFloor> LevelManager::prune(const std::vector<LevelFloor> &floors)
{
	std::vector<LevelFloor> pruned;

	for (const auto &floor : floors)
	{
		if (!floor.connectors.empty())
			pruned.push_back(floor);
	}

	return pruned;
}

std::vector<LevelFloor> LevelManager::generate_grid(const LevelFloor &start_floor, LevelSide start_side)
{
	std::vector<LevelFloor> generated;

	const int horizontal_tiles = random_int(3, 6);
	const int vertical_tiles = random_int(3, 6);
	const float tile_width = random_real(2.6f, 3.5f);
	const float tile_height = random_real(2.6f, 3.5f);

	float start_x, start_y;
	if (start_side == LevelSide::TOP)
	{
		start_x = (start_floor.x + (start_floor.w / 2.0f)) - ((horizontal_tiles * tile_width) / 2.0f);
		start_y = start_floor.y + start_floor.h;

		if (horizontal_tiles % 2 == 0)
			start_x -= (tile_width / 2.0f);
	}else if (start_side == LevelSide::LEFT)
	{
		start_x = start_floor.x - (horizontal_tiles * tile_width);
		start_y = (start_floor.y + (start_floor.h / 2.0f)) - ((vertical_tiles * tile_height) / 2.0f);

		if (vertical_tiles % 2 == 0)
			start_y -= (tile_height / 2.0f);
	}
	else if (start_side == LevelSide::RIGHT)
	{
		start_x = start_floor.x + start_floor.w;
		start_y = (start_floor.y + (start_floor.h / 2.0f)) - ((vertical_tiles * tile_height) / 2.0f);

		if (vertical_tiles % 2 == 0)
			start_y -= (tile_height / 2.0f);
	}
	else if (start_side == LevelSide::BOTTOM)
	{
		start_x = (start_floor.x + (start_floor.w / 2.0f)) - ((horizontal_tiles * tile_width) / 2.0f);
		start_y = start_floor.y - (vertical_tiles * tile_height);

		if (horizontal_tiles % 2 == 0)
			start_x -= (tile_width / 2.0f);
	}
	else win::bug("no side");

	const float initial_start_y = start_y;

	for (int i = 0; i < horizontal_tiles; ++i)
	{
		for (int j = 0; j < vertical_tiles; ++j)
		{
			const bool long_horizontal = random_int(0, 6) == 0;
			const bool long_vertical = random_int(0, 6) == 0;

			LevelFloor floor(random_int(0, 1), start_x, start_y, long_horizontal ? tile_width * 2.0f : tile_width, long_vertical ? tile_height * 2.0f : tile_height);

			bool collision = false;
			if (floor.collide(floors))
				collision = true;

			if (floor.collide(generated))
				collision = true;

			if (!collision)
				generated.push_back(floor);

			start_y += tile_height;
		}

		start_y = initial_start_y;
		start_x += tile_width;
	}

	auto neighbors = find_neighbors(generated, start_floor, start_side);
	if (neighbors.empty())
		return generated;
	auto neighbor = neighbors.at(random_int(0, neighbors.size() - 1));
	//if (!connect(start_floor, *neighbor)) return generated;
	LevelSide camefrom = flip(start_side);

	std::stack<LevelFloor*> heads;
	heads.push(neighbor);

	for (int i = 0; i < generated.size(); ++i)
	{
		auto head = heads.top();
		bool success = false;

		for (int attempt = 0; attempt < 2; ++attempt)
		{
			LevelSide dir = camefrom;
			while (dir == camefrom)
				dir = random_side();

			neighbors = find_neighbors(generated, *head, dir);
			if (neighbors.empty())
				continue;
			neighbor = neighbors.at(random_int(0, neighbors.size() - 1));

			if (connect(*head, *neighbor))
			{
				heads.push(neighbor);
				camefrom = flip(dir);
				success = true;
				break;
			}
		}

		if (!success)
		{
			heads.pop();
			if (heads.empty())
				break;
		}
	}

	return generated;
}

std::vector<LevelFloor> LevelManager::generate_linear(const LevelFloor &start_floor, const LevelSide start_side)
{
	std::vector<LevelFloor> generated;

	const int count = random_int(0, 3) == 0 ? random_int(2, 4) : random_int(5, 12);

	int from_index = -1;
	for (int num = 0; num < count; ++num)
	{
		LevelFloor *const head = from_index == -1 ? NULL : &generated.at(from_index);
		bool success = false;
		for (int attempt = 0; attempt < 6; ++attempt)
		{
			const LevelSide side = head == NULL ? start_side : random_side();

			const float from_x = head ? head->x : start_floor.x;
			const float from_y = head ? head->y : start_floor.y;
			const float from_w = head ? head->w : start_floor.w;
			const float from_h = head ? head->h : start_floor.h;

			const float width = random_int(0, 2) == 0 ? random_real(3.0f, 4.0f) : random_real(2.0f, 3.0f);
			const float height = random_int(0, 2) == 0 ? random_real(3.0f, 4.0f) : random_real(2.0f, 3.0f);
			float x, y;

			const float MIN_WALL_LENGTH = 1.8f; // minimum wall length needed to support a door (with some padding)

			const float lowx = (from_x + MIN_WALL_LENGTH) - width;
			const float highx = ((from_x + from_w) - MIN_WALL_LENGTH);

			const float lowy = (from_y + MIN_WALL_LENGTH) - height;
			const float highy = (from_y + from_h) - MIN_WALL_LENGTH;

			switch (side)
			{
			case LevelSide::LEFT:
				x = from_x - width;
				y = random_real(lowy, highy);
				break;
			case LevelSide::RIGHT:
				x = from_x + from_w;
				y = random_real(lowy, highy);
				break;
			case LevelSide::BOTTOM:
				x = random_real(lowx, highx);
				y = from_y - height;
				break;
			case LevelSide::TOP:
				x = random_real(lowx, highx);
				y = from_y + from_h;
				break;
			}

			LevelFloor floor(random_int(0, 1), x, y, width, height);

			if (floor.collide(floors))
				continue;
			if (floor.collide(generated))
				continue;

			if (head != NULL)
				if (!connect(floor, *head))
					win::bug("no connection");

			generated.push_back(floor);
			from_index = generated.size() - 1;

			success = true;
			break;
		}

		if (!success)
		{
			if (from_index == -1)
				return generated;

			--from_index;

			if (from_index == -1)
				return generated;
		}
	}

	return generated;
}

std::vector<LevelFloor*> LevelManager::find_neighbors(std::vector<LevelFloor> &floors, const LevelFloor& floor, LevelSide side)
{
	std::vector<LevelFloor*> matches;

	for (auto &f : floors)
	{
		if (&floor == &f)
			continue;

		LevelFloorConnector c1;
		LevelFloorConnector c2;

		if (can_connect(floor, f, c1, c2))
		{
			if (c1.side == side)
				matches.push_back(&f);
		}
	}

	return matches;
}

void LevelManager::generate_walls()
{
	for (const auto &floor : floors)
	{
		for (const auto side : enumerate_connector_sides())
		{
			auto connectors	= get_connectors(floor.connectors, side);

			if (side == LevelSide::BOTTOM || side == LevelSide::TOP)
			{
				const float y = side == LevelSide::BOTTOM ? floor.y - LevelWall::HALFWIDTH : ((floor.y + floor.h) - LevelWall::HALFWIDTH);
				float startx = floor.x;

				walls.emplace_back(startx, y, connectors.size() > 0 ? (connectors.at(0).start - startx) : floor.w, LevelWall::WIDTH);
				if (connectors.size() > 0)
					startx = connectors.at(0).stop;

				for (int i = 0; i < connectors.size(); ++i)
				{
					const auto &connector = connectors[i];
					walls.emplace_back(startx, y, i + 1 < connectors.size() ? connectors.at(i + 1).start - startx : ((floor.x + floor.w) - startx), LevelWall::WIDTH);
					if (i + 1 < connectors.size())
						startx = connectors.at(i + 1).stop;
				}
			}
			else
			{
				const float x = side == LevelSide::LEFT ? floor.x - LevelWall::HALFWIDTH : ((floor.x + floor.w) - LevelWall::HALFWIDTH);
				float starty = floor.y;

				walls.emplace_back(x, starty, LevelWall::WIDTH, connectors.size() > 0 ? (connectors.at(0).start - starty) : floor.h);
				if (connectors.size() > 0)
					starty = connectors.at(0).stop;

				for (int i = 0; i < connectors.size(); ++i)
				{
					const auto &connector = connectors[i];
					walls.emplace_back(x, starty, LevelWall::WIDTH, i + 1 < connectors.size() ? connectors.at(i + 1).start - starty : ((floor.y + floor.h) - starty));
					if (i + 1 < connectors.size())
						starty = connectors.at(i + 1).stop;
				}
			}
		}
	}
}

void LevelManager::generate_props()
{
	auto generate_excluder = [](const LevelFloor &f, const LevelFloorConnector &c)
	{
		const float excluder_half_width = 1.0f;
		const float excluder_width = excluder_half_width * 2.0f;

		float x, y, w, h;
		switch (c.side)
		{
		case LevelSide::LEFT:
			x = (c.side == LevelSide::RIGHT ? f.x + f.w : f.x) - excluder_half_width;
			y = c.start;
			w = excluder_width;
			h = c.stop - c.start;
			break;
		case LevelSide::TOP:
			x = c.start;
			y = (c.side == LevelSide::TOP ? f.y + f.h : f.y) - excluder_half_width;
			w = c.stop - c.start;
			h = excluder_width;
			break;
		case LevelSide::BOTTOM:
		case LevelSide::RIGHT:
			win::bug("invalid");
		}

		return LevelProp(false, x, y, w, h);
	};

	for (const auto &floor : floors)
	{

		const float w = 0.5f;
		const float h = 0.5f;

		//props.emplace_back(true, (floor.x + (floor.w / 2.0f)) - (w / 2.0f), (floor.y + (floor.h / 2.0f)) - (h / 2.0f), w, h);

		for (const auto &c : floor.connectors)
		{
			if (c.side == LevelSide::LEFT || c.side == LevelSide::TOP)
				props.push_back(generate_excluder(floor, c));
		}
	}
}

int LevelManager::random_int(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(mersenne);
}

float LevelManager::random_real(float low, float high)
{
	return std::uniform_real_distribution<float>(low, high)(mersenne);
}

LevelSide LevelManager::random_side()
{
	const int i = random_int(0, 3);

	switch (i)
	{
	case 0:
		return LevelSide::LEFT;
	case 1:
		return LevelSide::RIGHT;
	case 2:
		return LevelSide::BOTTOM;
	case 3:
		return LevelSide::TOP;
	}

	win::bug("no side");
}

bool LevelManager::can_connect(const LevelFloor &floor1, const LevelFloor &floor2, LevelFloorConnector &c1, LevelFloorConnector &c2)
{
	// determine which side floor 2 is on, relative to floor 1
	LevelSide side;
	if (float_equals(floor1.x + floor1.w, floor2.x)) // right?
		side = LevelSide::RIGHT;
	else if (float_equals(floor1.x, floor2.x + floor2.w)) // left?
		side = LevelSide::LEFT;
	else if (float_equals(floor1.y, floor2.y + floor2.h)) // bottom?
		side = LevelSide::BOTTOM;
	else if (float_equals(floor1.y + floor1.h, floor2.y)) // top?
		side = LevelSide::TOP;
	else
		return false;

	const float MIN_WALL_LENGTH = 1.8f; // minimum wall length needed to support a door (with some padding)
	const float DOOR_LENGTH = 1.0f;

	float start, stop;
	if (side == LevelSide::BOTTOM || side == LevelSide::TOP)
	{
		const float farleft = std::max(floor1.x, floor2.x);
		const float farright = std::min(floor1.x + floor1.w, floor2.x + floor2.w);
		if (farright - farleft < MIN_WALL_LENGTH)
			return false;

		start = ((farright + farleft) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}
	else if (side == LevelSide::LEFT || side == LevelSide::RIGHT)
	{
		const float lower = std::max(floor1.y, floor2.y);
		const float upper = std::min(floor1.y + floor1.h, floor2.y + floor2.h);

		if (upper - lower < MIN_WALL_LENGTH)
			return false;

		start = ((upper + lower) / 2.0f) - (DOOR_LENGTH / 2.0f);
		stop = start + DOOR_LENGTH;
	}

	LevelSide flipside;
	switch (side)
	{
	case LevelSide::TOP:
		flipside = LevelSide::BOTTOM;
		break;
	case LevelSide::BOTTOM:
		flipside = LevelSide::TOP;
		break;
	case LevelSide::LEFT:
		flipside = LevelSide::RIGHT;
		break;
	case LevelSide::RIGHT:
		flipside = LevelSide::LEFT;
		break;
	}

	LevelFloorConnector floor1_connector(side, start, stop);
	LevelFloorConnector floor2_connector(flipside, start, stop);

	if (floor1_connector.collide(floor1.connectors, 0.5f))
		return false;
	if (floor2_connector.collide(floor2.connectors, 0.5f))
		return false;

	c1 = floor1_connector;
	c2 = floor2_connector;

	return true;
}

bool LevelManager::connect(LevelFloor &floor1, LevelFloor &floor2)
{
	LevelFloorConnector floor1_connector;
	LevelFloorConnector floor2_connector;

	if (can_connect(floor1, floor2, floor1_connector, floor2_connector))
	{
		floor1.connectors.push_back(floor1_connector);
		floor2.connectors.push_back(floor2_connector);
		return true;
	}

	return false;
}

bool LevelManager::test_floor(const std::vector<LevelFloor> &floors, const LevelFloor &floor)
{
	for (const auto &f : floors)
	{
		if (f.collide(floor))
			return false;
	}

	return true;
}
