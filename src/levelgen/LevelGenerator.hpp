#pragma once

#include "../Darktimes.hpp"

#include "LevelObjectsInternal.hpp"
#include "../RandomNumberGenerator.hpp"

void level_generate(
	int seed,
	std::vector<LevelFloor> &floors,
	std::vector<LevelWall> &walls,
	std::vector<LevelProp> &props
);

class LevelGenerator
{
	NO_COPY_MOVE(LevelGenerator);

	typedef std::function<std::vector<LevelFloorInternal>(const LevelFloorInternal&, LevelSide)> GeneratorFunction;

public:
	LevelGenerator() = default;
	void generate(int seed);

	std::vector<LevelFloor> level_floors;
	std::vector<LevelWall> level_walls;
	std::vector<LevelProp> level_props;

private:
	std::vector<LevelFloorInternal> generate_impl();
	void generate_segment(std::vector<LevelFloorInternal> &existing_floors, const int minimum_added, const LevelSide side, const GeneratorFunction &generator);
	std::vector<LevelFloorInternal> generate_grid(const std::vector<LevelFloorInternal> &existing_floors, const LevelFloorInternal &start_floor, LevelSide start_side);
	std::vector<LevelFloorInternal> generate_linear(const std::vector<LevelFloorInternal> &existing_floors, const LevelFloorInternal &start_floor, LevelSide start_side);
	std::vector<LevelFloorInternal> generate_structure(const std::vector<LevelFloorInternal> &existing_floors, const LevelFloor &start_floor, LevelSide side, int index = -1);
	std::vector<int> find_start_candidates(const std::vector<LevelFloorInternal> &floors);
	static std::vector<int> find_neighbors(const std::vector<LevelFloorInternal> &floors, const LevelFloorInternal& floor);
	static bool connect(LevelFloorInternal &floor1, LevelFloorInternal &floor2);
	static bool can_connect(const LevelFloorInternal &floor1, const LevelFloorInternal &floor2, LevelFloorConnector &c1, LevelFloorConnector &c2);
	static std::vector<LevelFloorInternal> prune(const std::vector<LevelFloorInternal> &floors);
	static std::vector<LevelWallInternal> generate_walls(const std::vector<LevelFloorInternal> &floors);
	std::vector<LevelPropInternal> generate_props(const std::vector<LevelFloorInternal> &floors);
	static std::vector<LevelPropExcluder> generate_door_excluders(const LevelFloorInternal &floor);
	std::vector<LevelPropInternal> generate_new_props(const LevelFloorInternal &floor, const std::vector<LevelPropExcluder> &excluders);
	std::vector<LevelPropInternal> generate_props_from_spawns(const LevelFloorInternal &floor, const std::vector<LevelPropExcluder> &excluders);
	static std::vector<LevelPropInternal> generate_transition_strips(const LevelFloorInternal &floor);
	static LevelSide flip(LevelSide side);
	LevelSide random_side();
	static std::array<LevelSide, 4> enumerate_connector_sides();
	static std::vector<LevelFloorConnector> get_connectors(const std::vector<LevelFloorConnector> &connectors, LevelSide side);
	static bool float_equals(float a, float b, float tolerance = 0.005f);
	static void log_seed(int seed);

	RandomNumberGenerator rand;

	struct HealthReport
	{
		int restarts = 0;
		int iterations = 0;
	} health;
};
