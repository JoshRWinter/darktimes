#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include <vector>

struct LevelWall
{
	static constexpr float HALFWIDTH = 0.05f;

	LevelWall(float ax, float ay, float bx, float by)
		: ax(ax), ay(ay), bx(bx), by(by) {}

	float ax, ay, bx, by;
};

class LevelManager
{
public:
	NOCOPYMOVE(LevelManager);

	LevelManager();
	void reset();
	std::vector<float> get_wall_verts() const;

private:
	std::vector<LevelWall> walls;
};

#endif
