#pragma once

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "../../Darktimes.hpp"
#include "../TextureAssetMap.hpp"

class GLFloorTextureCollection
{
	NO_COPY_MOVE(GLFloorTextureCollection);

public:
	GLFloorTextureCollection(win::AssetRoll &roll, const TextureAssetMap &texture_map);

	const int &get_layer(Texture texture) const;

private:
	static std::unique_ptr<unsigned char[]> convert_8_to_24(const unsigned char *data, int width, int height);
	static std::unique_ptr<unsigned char[]> convert_32_to_24(const unsigned char *data, int width, int height);

	win::GLTexture tex;
	std::vector<int> map;
};
