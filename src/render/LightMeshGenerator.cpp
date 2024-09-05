#include <algorithm>

#include "LightMeshGenerator.hpp"

LightMeshGenerator::LightMeshGenerator()
{
}

void LightMeshGenerator::set_occluders(const std::vector<win::Box<float>> &occluders)
{
	float minx = 10000.0f, miny = 10000.0f, maxx = -10000.0f, maxy = -10000.0f;
	for (const auto &occluder : occluders)
	{
		minx = std::min(minx, occluder.x);
		miny = std::min(miny, occluder.y);
		maxx = std::max(maxx, occluder.x + occluder.width);
		maxy = std::max(maxy, occluder.y + occluder.height);
	}

	const float tolerance = 20.0f;

	index.reset(0.4f, minx - tolerance, maxx + tolerance, miny - tolerance, maxy + tolerance);

	this->occluders = occluders;

	for (auto &occluder : this->occluders)
		index.add(win::SpatialIndexLocation(occluder.x, occluder.y, occluder.width, occluder.height), occluder);
}

std::vector<float> LightMeshGenerator::generate(float centerx, float centery, float radius)
{
	std::vector<float> verts;
	std::vector<float> angles;
	std::vector<Line> lines;

	verts.push_back(centerx);
	verts.push_back(centery);

	// add some guard lines to constrain the mesh to the radius
	const float padding = 0.1f;
	lines.emplace_back(centerx - radius, (centery - radius) - padding, centerx - radius, centery + radius + padding); // left
	lines.emplace_back(centerx + radius, (centery - radius) - padding, centerx + radius, centery + radius + padding); // right
	lines.emplace_back(centerx - radius, centery - radius, centerx + radius, centery - radius); // bottom
	lines.emplace_back(centerx - radius, centery + radius, centerx + radius, centery + radius); // top

	// add some points to form a basic square around the player
	angles.push_back(get_angle(centerx, centery, centerx - radius, centery - radius));
	angles.push_back(get_angle(centerx, centery, centerx + radius, centery - radius));
	angles.push_back(get_angle(centerx, centery, centerx + radius, centery + radius));
	angles.push_back(get_angle(centerx, centery, centerx - radius, centery + radius));

	// decompose occluders into lines
	for (const auto &occluder : index.query(win::SpatialIndexLocation(centerx - radius, centery - radius, radius * 2.0f, radius * 2.0f)))
	{
		lines.emplace_back(occluder.x, occluder.y, occluder.x, occluder.y + occluder.height); // left
		lines.emplace_back(occluder.x + occluder.width, occluder.y, occluder.x + occluder.width, occluder.y + occluder.height); // right
		lines.emplace_back(occluder.x, occluder.y, occluder.x + occluder.width, occluder.y); // bottom
		lines.emplace_back(occluder.x, occluder.y + occluder.height, occluder.x + occluder.width, occluder.y + occluder.height); // top
	}

	// decompose occluders into points
	for (const auto &occluder : index.query(win::SpatialIndexLocation(centerx - radius, centery - radius, radius * 2.0f, radius * 2.0f)))
	{
		if (!near(occluder, centerx, centery, radius))
			continue;

		for (int i = 0; i < 3; ++i)
		{
			const float x1 = std::max(occluder.x, centerx - radius);
			const float y1 = std::max(occluder.y, centery - radius);
			const float x2 = std::min(occluder.x + occluder.width, centerx + radius);
			const float y2 = std::min(occluder.y + occluder.height, centery + radius);

			const float step = 0.0001f;
			const float offset = step * (i - 1);

			angles.push_back(get_angle(centerx, centery, x1, y1) + offset);
			angles.push_back(get_angle(centerx, centery, x2, y1) + offset);
			angles.push_back(get_angle(centerx, centery, x2, y2) + offset);
			angles.push_back(get_angle(centerx, centery, x1, y2) + offset);
		}
	}

	std::sort(angles.begin(), angles.end());

	for (const auto &angle : angles)
	{
		std::vector<Intersection> intersections;
		for (const auto &line: lines)
		{
			Intersection intersection;
			if (intersects(centerx, centery, angle, line, intersection))
				intersections.push_back(intersection);
		}

		if (intersections.empty())
		{
			win::bug("no intersections?");
		}
		else
		{
			std::sort(intersections.begin(), intersections.end(), [](const Intersection &a, const Intersection &b) { return a.dist < b.dist; });
			verts.push_back(intersections[0].x);
			verts.push_back(intersections[0].y);
		}
	}

	// add the first point again to cap it off
	verts.push_back(verts[2]);
	verts.push_back(verts[3]);

	return verts;
}

/*
bool LightMeshGenerator::intersects(float centerx, float centery, float angle, const Line &line, Intersection &intersection)
{
	const float ray_vec_x = std::cosf(angle);
	const float ray_vec_y = std::sinf(angle);

	// check for parallel
	const float r_mag = sqrtf((ray_vec_x * ray_vec_x) + (ray_vec_y * ray_vec_y));
	const float s_mag = sqrtf(((line.x2 - line.x1) * (line.x2 - line.x1)) + ((line.y2 - line.y1) * (line.y2 - line.y1)));
	if(ray_vec_x / r_mag == (line.x2 - line.x1) / s_mag && ray_vec_y / r_mag == (line.y2 - line.y1) / s_mag)
	{
		// the lines are parallel
		return false;
	}

	const float t2 = (ray_vec_x * (line.y1 - centery) + ray_vec_y * (centerx - line.x1)) / ((line.x2 - line.x1) * ray_vec_y - (line.y2 - line.y1) * ray_vec_x);
	const float t1 = (line.x1 + (line.x2 - line.x1) * t2 - centerx) / ray_vec_x;

	if(t1 < 0.0f || (t2 < 0.0f || t2 > 1.0f))
	{
		return false;
	}

	intersection = Intersection(centerx + ray_vec_x * t1, centery + ray_vec_y * t1, t1);
	return true;
}
 */

bool LightMeshGenerator::intersects(float centerx, float centery, float angle, const Line &line, Intersection &intersection)
{
	const float r_px = centerx;
	const float r_py = centery;
	const float r_dx = std::cosf(angle);
	const float r_dy = std::sin(angle);

	const float s_px = line.x1;
	const float s_py = line.y1;
	const float s_dx = line.x2 - line.x1;
	const float s_dy = line.y2 - line.y1;

	// check for parellel
	const float r_mag = sqrtf((r_dx * r_dx) + (r_dy * r_dy));
	const float s_mag = sqrtf((s_dx * s_dx) + (s_dy * s_dy));
	if(r_dx / r_mag == s_dx / s_mag && r_dy / r_mag == s_dy / s_mag)
	{
		// the lines are parellel
		return false;
	}

	const float t2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
	const float t1 = (s_px + s_dx * t2 - r_px) / r_dx;

	if(t1 < 0.0f || (t2 < 0.0f || t2 > 1.0f))
	{
		return false;
	}

	intersection.x = r_px + r_dx * t1;
	intersection.y = r_py + r_dy * t1;
	intersection.dist = t1;

	return true;
}

bool LightMeshGenerator::near(const win::Box<float> &box, float centerx, float centery, float radius)
{
	return box.x + box.width >= centerx - radius && box.x <= centerx + radius && box.y + box.height >= centery - radius && box.y <= centery + radius;
}

float LightMeshGenerator::get_angle(float centerx, float centery, float x, float y)
{
	x -= centerx;
	y -= centery;

	return std::atan2f(y, x);
}
