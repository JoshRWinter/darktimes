#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <vector>
#include <random>

#include "../darktimes.hpp"

enum class LevelSide
{
	LEFT,
	RIGHT,
	BOTTOM,
	TOP
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

	bool collide(const LevelFloor &rhs) const { return x + w > rhs.x && x < rhs.x + rhs.w && y + h > rhs.y && y < rhs.y + rhs.h; }
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
	const int seed;

private:
	int random_int(int, int);
	float random_real(float, float);
	LevelSide random_side();

	bool generate_grid();
	bool generate_linear();
	void reset();
	void prune();

	static bool can_connect(LevelFloor&, LevelFloor&, LevelFloorConnector&, LevelFloorConnector&);
	static bool connect(LevelFloor&, LevelFloor&);
	LevelFloor *find_neighbor(LevelFloor&, LevelSide);
	void generate_walls();
	static bool test_floor(const std::vector<LevelFloor>&, const LevelFloor&);

	std::mt19937 mersenne;
};

#endif
