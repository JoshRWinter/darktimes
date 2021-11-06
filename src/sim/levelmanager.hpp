#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <vector>
#include <random>

struct LevelWall
{
	static constexpr float HALFWIDTH = 0.05f;

	LevelWall(float ax, float ay, float bx, float by)
		: ax(ax), ay(ay), bx(bx), by(by) {}

	float ax, ay, bx, by;
};

struct LevelFloor
{
	LevelFloor(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h) {}

	float x, y, w, h;
};

class LevelManager
{
public:
	NOCOPYMOVE(LevelManager);

	LevelManager();
	void reset();
	std::vector<float> get_wall_verts() const;
	std::vector<float> get_floor_verts();

private:
	int random_int(int, int);

	std::mt19937 mersenne;
	std::vector<LevelFloor> floors;
	std::vector<LevelWall> walls;
};

#endif
