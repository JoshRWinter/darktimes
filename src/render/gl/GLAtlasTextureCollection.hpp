#pragma once

#include <vector>

#include <win/AssetRoll.hpp>
#include <win/gl/GLAtlas.hpp>

#include "../../Darktimes.hpp"
#include "../TextureAssetMap.hpp"

class GLAtlasTextureCollection
{
	NO_COPY_MOVE(GLAtlasTextureCollection);

public:
	GLAtlasTextureCollection(win::AssetRoll &roll, const TextureAssetMap &texture_map);

	const win::GLAtlas &get_atlas(Texture texture) const;

private:
	const win::GLAtlas *find(const char *asset_path, const TextureAssetMap &texture_map) const;

	std::vector<const win::GLAtlas*> map;
	std::vector<win::GLAtlas> atlases;
};
