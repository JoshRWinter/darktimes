#ifndef PROP_DEFS_HPP
#define PROP_DEFS_HPP

#include <vector>

#include "levelobjects.hpp"

class PropDefinitions
{
public:
	inline static std::vector<LevelPropDefinition> side_tables =
	{
		LevelPropDefinition(0, true, 0.36f, 0.7f)
	};

	inline static std::vector<LevelPropDefinition> center_tables =
	{
		LevelPropDefinition(0, true, 0.5f, 0.5f, 0.1f, 0.1f)
	};

	inline static std::vector<LevelPropDefinition> rugs =
	{
		LevelPropDefinition(0, false, 1.0f, 2.0f, 0.0f, 0.0f)
	};
};

#endif
