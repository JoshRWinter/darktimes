#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <vector>
#include <random>
#include <list>

#include "../darktimes.hpp"

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
	enum class Side
	{
		LEFT, RIGHT, BOTTOM, TOP
	};

	LevelFloorConnector(LevelFloor *link, Side side, float start, float stop)
		: side(side), link(link), start(start), stop(stop) {}

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

	LevelFloor *link;
	Side side;
	float start, stop;
};

struct LevelFloor
{
	LevelFloor(int texture, float x, float y, float w, float h)
		: texture(texture), x(x), y(y), w(w), h(h) {}

	bool collide(const LevelFloor &rhs) const { return x + w > rhs.x && x < rhs.x + rhs.w && y + h > rhs.y && y < rhs.y + rhs.h; }
	bool collide(const std::list<LevelFloor> &rhs) const
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
	void reset();

	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;

private:
	int random_int(int, int);
	float random_real(float, float);

	void generate_grid();
	void generate_linear();

	bool connect(LevelFloor&, LevelFloor&);
	void generate_walls();
	static bool test_floor(const std::vector<LevelFloor>&, const LevelFloor&);

	std::mt19937 mersenne;
};

#endif
