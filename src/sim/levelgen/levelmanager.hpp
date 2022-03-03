#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <random>

#include "../../darktimes.hpp"
#include "../../randomnumbergenerator.hpp"
#include "levelobjects.hpp"

class LevelManager
{
public:
	NOCOPYMOVE(LevelManager);

	LevelManager(int);
	void generate();

	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;
	std::vector<LevelProp> props;
	const int seed;

private:
	LevelSide random_side();

	bool generate_impl();
	std::vector<LevelFloor> generate_grid(const LevelFloor&, LevelSide);
	std::vector<LevelFloor> generate_linear(const LevelFloor&, LevelSide);
	std::vector<LevelFloor> generate_structure(const LevelFloor&, LevelSide);
	void reset();

	int find_start_candidate(const std::vector<LevelFloor>&, int);
	static std::vector<LevelFloor> prune(const std::vector<LevelFloor>&);
	static bool can_connect(const LevelFloor&, const LevelFloor&, LevelFloorConnector&, LevelFloorConnector&);
	static bool connect(LevelFloor&, LevelFloor&);
	static std::vector<LevelFloor*> find_neighbors(std::vector<LevelFloor>&, const LevelFloor&, LevelSide);
	void generate_walls();
	void generate_props();
	std::vector<LevelProp> generate_props(const LevelFloor&, const std::vector<LevelProp> &excluders);
	std::vector<LevelProp> generate_door_excluders(const LevelFloor&);
	static bool test_floor(const std::vector<LevelFloor>&, const LevelFloor&);

	RandomNumberGenerator rand;
};

#endif
