#pragma once

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Darktimes.hpp"
#include "../TextureAssetMap.hpp"

class FloorTextureCollection
{
	NO_COPY_MOVE(FloorTextureCollection);

public:
	FloorTextureCollection(win::AssetRoll &roll, const TextureAssetMap &texture_map);

	const int &get_layer(Texture texture) const;

private:
	win::GLTexture tex;
	std::vector<int> map;
};
