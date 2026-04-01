#pragma once

#include <win/Win.hpp>

#include "../Texture.hpp"

#if defined (STATIC) || defined (DYNAMIC) || defined STATIC_DYNAMIC
#error "macro confusion"
#endif

#define STATIC false
#define DYNAMIC true
#define STATIC_DYNAMIC true

#define ATLAS_TEXTURE(textureid, assetpath, atlasindex, type) const bool hidden_##textureid = add(Texture::textureid, assetpath, atlasindex, type)
#define FLOOR_TEXTURE(textureid, assetpath, type) const bool hidden_##textureid = add(Texture::textureid, assetpath, -1, type)

struct TextureMapItem
{
	const char *asset_path = NULL;
	int atlas_index = -1;
	bool dynamic = false;
};

class TextureAssetMap
{
	WIN_NO_COPY_MOVE(TextureAssetMap);

public:
	TextureAssetMap() = default;

	static constexpr int size()
	{
		return static_cast<int>(Texture::max_texture);
	}

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
		return map[(int)texture];
#endif
	}

private:
	TextureMapItem map[(int)Texture::max_texture];

	bool add(Texture textureid, const char *asset_path, int atlas_index, bool dynamic)
	{
		TextureMapItem tmi;
		tmi.asset_path = asset_path;
		tmi.atlas_index = atlas_index;
		tmi.dynamic = dynamic;

		map[(int)textureid] = tmi;

		return false;
	}

#include "TextureAssetMap.txt"
};

#undef STATIC
#undef DYNAMIC
#undef STATIC_DYNAMIC
#undef ATLAS_TEXTURE
#undef FLOOR_TEXTURE
