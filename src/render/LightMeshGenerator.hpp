#pragma once

#include <vector>

#include <win/BlockMap.hpp>
#include <win/Win.hpp>
#include <win/Utility.hpp>

class LightMeshGenerator
{
	WIN_NO_COPY_MOVE(LightMeshGenerator);

	struct Point
	{
		Point(float x, float y, float angle) : x(x), y(y), angle(angle) {}
		float x, y, angle;
	};

	struct Line
	{
		Line(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
		float x1, y1, x2, y2;
	};

	struct Intersection
	{
		Intersection() = default;
		Intersection(float x, float y, float dist) : x(x), y(y), dist(dist) {}
		float x, y, dist;
	};

public:
	LightMeshGenerator();

	void set_occluders(const std::vector<win::Box<float>> &occluders);
	std::vector<float> generate(float centerx, float centery, float radius);

private:
	static bool intersects(float centerx, float centery, float angle, const Line &line, Intersection &intersection);
	static bool near(const win::Box<float> &box, float centerx, float centery, float radius);
	static float get_angle(float centerx, float centery, float x, float y);

	win::BlockMap<win::Box<float>> index;
	std::vector<win::Box<float>> occluders;
};
