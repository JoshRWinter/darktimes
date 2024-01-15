#include "Structure.hpp"

void correct_structure_properties(const LevelSide side, const float origin_x, const float origin_y, const float source_x, const float source_y, const float source_width, const float source_height, float &out_x, float &out_y, float &out_width, float &out_height)
{
	switch (side)
	{
	case LevelSide::top:
	{
		const float rotated_x = source_x;
		const float rotated_y = source_y;
		const float translated_x = rotated_x + origin_x;
		const float translated_y = rotated_y + origin_y;
		const float corner_corrected_x = translated_x;
		const float corner_corrected_y = translated_y;

		out_x = corner_corrected_x;
		out_y = corner_corrected_y;
		out_width = source_width;
		out_height = source_height;
		break;
	}
	case LevelSide::left:
	{
		const float rotated_x = -source_y;
		const float rotated_y = source_x;
		const float translated_x = rotated_x + origin_x;
		const float translated_y = rotated_y + origin_y;
		const float corner_corrected_x = translated_x - source_height;
		const float corner_corrected_y = translated_y;

		out_x = corner_corrected_x;
		out_y = corner_corrected_y;
		out_width = source_height;
		out_height = source_width;
		break;
	}
	case LevelSide::bottom:
	{
		const float rotated_x = -source_x;
		const float rotated_y = -source_y;
		const float translated_x = rotated_x + origin_x;
		const float translated_y = rotated_y + origin_y;
		const float corner_corrected_x = translated_x - source_width;
		const float corner_corrected_y = translated_y - source_height;

		out_x = corner_corrected_x;
		out_y = corner_corrected_y;
		out_width = source_width;
		out_height = source_height;
		break;
	}
	case LevelSide::right:
	{
		const float rotated_x = source_y;
		const float rotated_y = -source_x;
		const float translated_x = rotated_x + origin_x;
		const float translated_y = rotated_y + origin_y;
		const float corner_corrected_x = translated_x;
		const float corner_corrected_y = translated_y - source_width;

		out_x = corner_corrected_x;
		out_y = corner_corrected_y;
		out_width = source_height;
		out_height = source_width;
		break;
	}
	}
}
