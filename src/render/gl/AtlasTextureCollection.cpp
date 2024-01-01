#include <cstring>

#include "AtlasTextureCollection.hpp"

AtlasTextureCollection::AtlasTextureCollection(win::AssetRoll &roll, const TextureMap &texture_map)
{
	std::vector<const char*> seen;
	map.reserve((int)Texture::max_texture);

	for (int i = 0; i < (int)Texture::max_texture; ++i)
	{
		const auto &item = texture_map[(Texture)i];
		if (item.atlas_index != -1)
		{
			auto atlas = find(item.asset_path, texture_map);
			if (atlas == NULL)
				atlas = &atlases.emplace_back(roll[item.asset_path], win::GLAtlas::Mode::linear);

			map.push_back(atlas);
		}
		else
			map.push_back(NULL);
	}

#ifndef NDEBUG
	win::gl_check_error();
#endif
}

const win::GLAtlas &AtlasTextureCollection::get_atlas(Texture texture) const
{
	return *map[(int)texture];
}

const win::GLAtlas *AtlasTextureCollection::find(const char *asset_path, const TextureMap &texture_map) const
{
	for (int i = 0; i < map.size(); ++i)
	{
		const auto &item = texture_map[(Texture)i];
		if (item.atlas_index != -1)
		{
			if (!strcmp(item.asset_path, asset_path))
				return map.at(i);
		}
	}

	return NULL;
}
