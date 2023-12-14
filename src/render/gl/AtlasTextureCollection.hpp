#pragma once

#include <vector>

#include <win/AssetRoll.hpp>
#include <win/gl/GLAtlas.hpp>

#include "../../Darktimes.hpp"

class AtlasTextureCollection
{
	NO_COPY_MOVE(AtlasTextureCollection);

public:
	explicit AtlasTextureCollection(win::AssetRoll &roll);

	const std::vector<win::GLAtlas*> &get_atlas_map() const;

private:
	std::vector<win::GLAtlas> atlases;
	std::vector<win::GLAtlas*> map;
};
