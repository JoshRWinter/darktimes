#pragma once

#include <vector>

#include <win/AssetRoll.hpp>
#include <win/gl/GLAtlas.hpp>

#include "../../Darktimes.hpp"
#include "../TextureMap.hpp"

class AtlasTextureCollection
{
	NO_COPY_MOVE(AtlasTextureCollection);

public:
	AtlasTextureCollection(win::AssetRoll &roll, const TextureMap &texture_map);

	const win::GLAtlas &get_atlas(Texture texture) const;

private:
	const win::GLAtlas *find(const char *asset_path, const TextureMap &texture_map) const;

	std::vector<const win::GLAtlas*> map;
	std::vector<win::GLAtlas> atlases;
};
