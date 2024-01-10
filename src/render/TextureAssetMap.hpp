#pragma once

#include <win/Win.hpp>

#include "../Texture.hpp"

#define ATLAS(name, assetpath) const char *name = assetpath

#if defined (STATIC) || defined (DYNAMIC) || defined STATIC_DYNAMIC
#error "macro confusion"
#endif

#define STATIC false
#define DYNAMIC true
#define STATIC_DYNAMIC true

#define ATLAS_TEXTURE(textureid, assetpath, atlasindex, type) const bool hidden_##textureid = add(Texture::textureid, assetpath, atlasindex, -1, type)
#define FLOOR_TEXTURE(textureid, assetpath, layerindex, type) const bool hidden_##textureid = add(Texture::textureid, assetpath, -1, layerindex, type)

struct TextureMapItem
{
	const char *asset_path = NULL;
	int layer_index = -1;
	int atlas_index = -1;
	bool dynamic = false;
};

class TextureAssetMap
{
public:
	const TextureMapItem &operator[](Texture texture) const
	{
#ifndef NDEBUG
		if (texture < (Texture)0 || texture >= Texture::max_texture)
			win::bug("out of range");

		const auto &t = map[(int)texture];
		if (t.asset_path == NULL)
			win::bug("Texture " + std::to_string((int)texture) + " is not initialized in the texture map.");

		return t;
#else
		return static_textures[(int)texture];
#endif
	}

private:
	TextureMapItem map[(int)Texture::max_texture];

	bool add(Texture textureid, const char *asset_path, int atlas_index, int layer_index, bool dynamic)
	{
		TextureMapItem tmi;
		tmi.asset_path = asset_path;
		tmi.atlas_index = atlas_index;
		tmi.layer_index = layer_index;
		tmi.dynamic = dynamic;

		map[(int)textureid] = tmi;

		return false;
	}

#include "TextureAssetMap.txt"
};

#undef ATLAS
#undef STATIC
#undef DYNAMIC
#undef STATIC_DYNAMIC
#undef ATLAS_TEXTURE
#undef FLOOR_TEXTURE
