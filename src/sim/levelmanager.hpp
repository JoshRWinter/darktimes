#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <vector>
#include <random>
#include <list>

#include "../darktimes.hpp"

struct LevelWall
{
	static constexpr float HALFWIDTH = 0.05f;

	LevelWall(float ax, float ay, float bx, float by)
		: ax(ax), ay(ay), bx(bx), by(by) {}

	float ax, ay, bx, by;
};

struct LevelFloor
{
	LevelFloor(int texture, float x, float y, float w, float h)
		: texture(texture), x(x), y(y), w(w), h(h) {}

	bool collide(const LevelFloor &rhs) const { return x + w > rhs.x && x < rhs.x + rhs.w && y + h > rhs.y && y < rhs.y + rhs.h; }

	int texture; float x, y, w, h;

	std::vector<int> rel_left, rel_right, rel_bottom, rel_top;
};

class LevelManager
{
public:
	NOCOPYMOVE(LevelManager);

	LevelManager(int);
	void reset();

	std::vector<LevelFloor> floors;

private:
	int random_int(int, int);
	float random_real(float, float);
	bool connect_new(std::vector<LevelFloor>&, int);
	static bool test_floor(const std::vector<LevelFloor>&, const LevelFloor&);

	std::mt19937 mersenne;
};

#endif
