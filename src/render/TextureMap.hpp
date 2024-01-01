#pragma once

#include <win/Win.hpp>

#include "../Texture.hpp"

#define ATLAS(name, assetpath) const char *name = assetpath
#define ATLAS_TEXTURE(textureid, assetpath, atlasindex) const bool hidden_##textureid = add(Texture::textureid, assetpath, atlasindex, -1)
#define FLOOR_TEXTURE(textureid, assetpath, layerindex) const bool hidden_##textureid = add(Texture::textureid, assetpath, -1, layerindex)

struct TextureMapItem
{
	const char *asset_path = NULL;
	int layer_index = -1;
	int atlas_index = -1;
};

class TextureMap
{
public:
	const TextureMapItem &operator[](Texture texture) const
	{
#ifndef NDEBUG
		const auto &t = map[(int)texture];
		if (t.asset_path == NULL)
			win::bug("Texture " + std::to_string((int)texture) + " is not initialized in the texture map.");

		return t;
#else
		return map[(int)texture];
#endif
	}

private:
	TextureMapItem map[(int)Texture::max_texture];

	bool add(Texture textureid, const char *asset_path, int atlas_index, int layer_index)
	{
		TextureMapItem tmd;
		tmd.asset_path = asset_path;
		tmd.atlas_index = atlas_index;
		tmd.layer_index = layer_index;

		map[(int)textureid] = tmd;

		return false;
	}

#include "TextureMap.txt"

};

#undef ATLAS
#undef ATLAS_TEXTURE
#undef FLOOR_TEXTURE
