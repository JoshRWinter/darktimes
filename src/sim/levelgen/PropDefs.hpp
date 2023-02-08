#pragma once

#include <vector>

#include "LevelObjects.hpp"

class PropDefinitions
{
public:
	inline static std::vector<LevelPropDefinition> side_tables =
	{
		LevelPropDefinition(0, LevelPropDefinition::collision_class_furniture, LevelPropDefinition::collision_class_excluder | LevelPropDefinition::collision_class_furniture, 0.36f, 0.7f)
	};

	inline static std::vector<LevelPropDefinition> center_tables =
	{
		LevelPropDefinition(0, LevelPropDefinition::collision_class_furniture, LevelPropDefinition::collision_class_excluder | LevelPropDefinition::collision_class_furniture, 0.5f, 0.5f, 0.1f, 0.1f)
	};

	inline static std::vector<LevelPropDefinition> rugs =
	{
		LevelPropDefinition(0, LevelPropDefinition::collision_class_rug, LevelPropDefinition::collision_class_rug, 1.0f, 2.0f, 0.0f, 0.0f)
	};

	inline static LevelPropDefinition floor_transition_strip = LevelPropDefinition(0, LevelPropDefinition::collision_class_debris, 0.0f, 0.025f, 1.0f, 0.0f, 0.0f);
};
