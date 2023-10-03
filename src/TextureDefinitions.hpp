#pragma once

#include <vector>

struct TextureDefinition
{
	TextureDefinition(const char *resource, int atlas, int atlas_index)
		: resource(resource)
		, atlas(atlas)
		, atlas_index(atlas_index)
	{}

	const char *resource;
	int atlas;
	int atlas_index;
};

#define ATLAS(name, path) private: const int hidden_atlas_##name = add_atlas(path); public:
#define TEXTURE(name, atlas, index) const int name = add_atlas_texture(hidden_atlas_##atlas, index);
#define FLOOR_TEXTURE(name, resource) const int name = add_floor_texture(resource);

class TextureDefinitionsStorage
{
public:
	std::vector<const char*> atlases;
	std::vector<TextureDefinition> textures;
	std::vector<int> floor_textures;
	std::vector<int> atlas_items;

#include "Textures.txt"

private:
	int add_atlas(const char *path)
	{
		atlases.push_back(path);
		return atlases.size() - 1;
	}

	int add_atlas_texture(int atlas, int index)
	{
		textures.emplace_back(atlases.at(atlas), atlas, index);
		atlas_items.push_back(textures.size() - 1);
		return textures.size() - 1;
	}

	int add_floor_texture(const char *resource)
	{
		textures.emplace_back(resource, -1, -1);
		floor_textures.push_back(textures.size() - 1);
		return textures.size() - 1;
	}
};

#ifndef TEXTURE_DEFINITIONS_CPP
extern const TextureDefinitionsStorage TextureDefinitions;
#endif

#undef ATLAS
#undef TEXTURE
#undef FLOOR_TEXTURE
