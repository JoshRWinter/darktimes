#pragma once

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Darktimes.hpp"

class FloorTextureCollection
{
	NO_COPY_MOVE(FloorTextureCollection);

public:
	explicit FloorTextureCollection(win::AssetRoll &roll);

	const std::vector<int> &get_layer_map() const;

private:
	win::GLTexture tex;
	std::vector<int> map;
};
