#ifndef STRUCTURE_HPP
#define STRUCTURE_HPP

#include <vector>

#include <win/win.hpp>
#include "levelobjects.hpp"

enum class StructureOrientation
{
	up,
	down,
	left,
	right
};

struct StructureFloor
{
	StructureFloor(int texture, float x, float y, float width, float height)
		: texture(texture), x(x), y(y), width(width), height(height) {}

	int texture;
	float x, y;
	float width, height;
};

struct StructureFloorConnection
{
	StructureFloorConnection(int x, int y)
		: index_x(x), index_y(y) {}

	int index_x;
	int index_y;
};

struct Structure
{
	std::vector<StructureFloor> floors;
	std::vector<StructureFloorConnection> connections;
};

#endif
