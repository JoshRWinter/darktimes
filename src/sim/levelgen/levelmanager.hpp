#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <vector>
#include <random>

#include "../../darktimes.hpp"

struct LevelProp
{
	LevelProp(bool collide, float x, float y, float w, float h)
		: collide(collide), x(x), y(y), w(w), h(h) {}

	bool collide;
	float x, y, w, h;
};

enum class LevelSide
{
	left,
	right,
	bottom,
	top
};

struct LevelWall
{
	static constexpr float HALFWIDTH = 0.05f;
	static constexpr float WIDTH = HALFWIDTH * 2.0f;

	LevelWall(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h) {}

	float x, y, w, h;
};

struct LevelFloor;
struct LevelFloorConnector
{
	LevelFloorConnector() = default;

	LevelFloorConnector(LevelSide side, float start, float stop)
		: side(side), start(start), stop(stop) {}

	bool collide(const LevelFloorConnector &rhs, float padding) const
	{
		if (side != rhs.side)
			return false;

		const bool above = rhs.start > stop + padding;
		const bool below = rhs.stop < start - padding;

		if (above || below)
			return false;

		return true;
	}

	bool collide(const std::vector<LevelFloorConnector> &rhs, float padding) const
	{
		for (const auto &x : rhs)
			if (collide(x, padding))
				return true;

		return false;
	}

	LevelSide side;
	float start, stop;
};

struct LevelFloor
{
	LevelFloor(int texture, float x, float y, float w, float h)
		: texture(texture), x(x), y(y), w(w), h(h) {}

	bool collide(const LevelFloor &rhs, float tolerance = 0.05f) const { return x + w > rhs.x + tolerance && x < (rhs.x + rhs.w) - tolerance && y + h > rhs.y + tolerance && y < (rhs.y + rhs.h) - tolerance; }
	bool collide(const std::vector<LevelFloor> &rhs) const
	{
		for (const auto &x : rhs)
			if (collide(x))
				return true;

		return false;
	}

	int texture; float x, y, w, h;

	std::vector<LevelFloorConnector> connectors;
};

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
	int random_int(int, int);
	float random_real(float, float);
	LevelSide random_side();

	std::vector<LevelFloor> generate_grid(const LevelFloor&, LevelSide);
	std::vector<LevelFloor> generate_linear(const LevelFloor&, LevelSide);
	void reset();

	bool generate_impl();
	int find_start_candidate(const std::vector<LevelFloor>&, int);
	static std::vector<LevelFloor> prune(const std::vector<LevelFloor>&);
	static bool can_connect(const LevelFloor&, const LevelFloor&, LevelFloorConnector&, LevelFloorConnector&);
	static bool connect(LevelFloor&, LevelFloor&);
	static std::vector<LevelFloor*> find_neighbors(std::vector<LevelFloor>&, const LevelFloor&, LevelSide);
	void generate_walls();
	void generate_props();
	static bool test_floor(const std::vector<LevelFloor>&, const LevelFloor&);

	std::mt19937 mersenne;
};

#endif
