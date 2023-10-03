#include "AtlasTextureCollection.hpp"

#include "../../TextureDefinitions.hpp"

AtlasTextureCollection::AtlasTextureCollection(win::AssetRoll &roll)
{
	map.resize(TextureDefinitions.textures.size(), NULL);

	for (const auto name : TextureDefinitions.atlases)
		atlases.emplace_back(roll[name], win::GLAtlas::Mode::linear);

	for (const auto item : TextureDefinitions.atlas_items)
		map[item] = &atlases.at(TextureDefinitions.textures[item].atlas);
}

const std::vector<win::GLAtlas*> &AtlasTextureCollection::get_atlas_map() const
{
	return map;
}
